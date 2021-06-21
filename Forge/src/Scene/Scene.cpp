#include "ForgePch.h"
#include "Scene.h"

#include "Entity.h"
#include "Transform.h"
#include "CameraComponent.h"
#include "ModelRenderer.h"
#include "AnimatorComponent.h"
#include "Components.h"

namespace Forge
{

    Scene::Scene(const Ref<Framebuffer>& defaultFramebuffer, Renderer3D* renderer)
        : m_Registry(), m_PrimaryCamera(entt::null), m_Time(0.0f), m_Renderer(renderer), m_DefaultFramebuffer(defaultFramebuffer), m_PickFramebuffer(nullptr)
    {
    }

    Entity Scene::GetPrimaryCamera()
    {
        FindPrimaryCamera();
        return Entity(m_PrimaryCamera, this);
    }

    Entity Scene::CreateEntity(uint8_t layer)
    {
        return CreateEntity("<Unnamed>", layer);
    }

    Entity Scene::CreateEntity(const std::string& name, uint8_t layer)
    {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<LayerId>(1ULL << layer);
        entity.AddComponent<TagComponent>(name);
        entity.SetEnabled(true);
        return entity;
    }

    void Scene::DestroyEntity(const Entity& entity)
    {
        m_Registry.destroy(entity);
    }

    void Scene::SetLayer(Entity entity, uint8_t layer)
    {
        entity.GetComponent<LayerId>().Mask = 1ULL << layer;
    }

    void Scene::AddLayer(Entity entity, uint8_t layer)
    {
        entity.GetComponent<LayerId>().Mask |= 1ULL << layer;
    }

    void Scene::RemoveLayer(Entity entity, uint8_t layer)
    {
        entity.GetComponent<LayerId>().Mask &= ~(1ULL << layer);
    }

    void Scene::AddToAllLayers(Entity entity)
    {
        entity.GetComponent<LayerId>().Mask = -1;
    }

    void Scene::SetPrimaryCamera(const Entity& entity)
    {
        FORGE_ASSERT(entity.HasComponent<CameraComponent>(), "Camera must have a CameraComponent");
        m_PrimaryCamera = entity;
    }

    Entity Scene::CreateCamera(const Frustum& frustum)
    {
        Entity camera = CreateEntity("Camera");
        camera.AddComponent<CameraComponent>(frustum).Viewport = { 0, 0, m_DefaultFramebuffer->GetWidth(), m_DefaultFramebuffer->GetHeight() };
        return camera;
    }

    Entity Scene::PickEntity(const glm::vec2& viewportCoord, const Entity& camera)
    {
        CameraData data;
        auto [transform, cameraComponent, enabled] = m_Registry.get<TransformComponent, CameraComponent, EnabledFlag>(camera);
        data.Frustum = cameraComponent.Frustum;
        data.ViewMatrix = transform.GetInverseMatrix();
        data.Viewport = cameraComponent.Viewport;
        data.ClippingPlanes = cameraComponent.ClippingPlanes;
        data.ClearColor = cameraComponent.ClearColor;
        data.Mode = cameraComponent.Mode;

        if (!m_PickFramebuffer)
        {
            FramebufferProps props;
            props.Width = data.Viewport.Width;
            props.Height = data.Viewport.Height;
            props.Attachments = { FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
            m_PickFramebuffer = Framebuffer::Create(props);
        }
        else if (m_PickFramebuffer->GetWidth() != data.Viewport.Width || m_PickFramebuffer->GetHeight() != data.Viewport.Height)
        {
            m_PickFramebuffer->SetSize(data.Viewport.Width, data.Viewport.Height);
        }

        m_PickFramebuffer->Bind();
        RenderCommand::Clear();
        m_PickFramebuffer->ClearAttachment(0, -1);
        m_Renderer->BeginPickScene(m_PickFramebuffer, data);
        for (auto entity : m_Registry.view<TransformComponent, ModelRendererComponent, EnabledFlag>())
        {
            if (CheckLayerMask(entity, cameraComponent.LayerMask))
            {
                auto [transform, model] = m_Registry.get<TransformComponent, ModelRendererComponent>(entity);
                RenderOptions options;
                options.EntityId = (int)entity;
                m_Renderer->RenderModel(model.Model, transform.GetMatrix(), options);
            }
        }
        m_Renderer->EndScene();

        int pixel = m_PickFramebuffer->ReadPixel(0, viewportCoord.x, viewportCoord.y);
        return Entity((entt::entity)pixel, this);
    }

    void Scene::OnUpdate(Timestep ts)
    {
        static std::vector<LightSource> s_LightSources;
        static std::vector<uint64_t> s_LightSourceShadowLayerMasks;

        m_Time += ts.Seconds();

        auto cameraView = m_Registry.view<TransformComponent, CameraComponent, EnabledFlag>();
        std::vector<entt::entity> cameras = { cameraView.begin(), cameraView.end() };
        std::sort(cameras.begin(), cameras.end(), [this](entt::entity a, entt::entity b)
        {
            CameraComponent& ccA = m_Registry.get<CameraComponent>(a);
            CameraComponent& ccB = m_Registry.get<CameraComponent>(b);
            if (ccA.RenderTarget == nullptr && ccB.RenderTarget != nullptr)
                return false;
            if (ccB.RenderTarget == nullptr && ccA.RenderTarget != nullptr)
                return true;
            if (ccA.Mode == CameraMode::Overlay && ccB.Mode != CameraMode::Overlay)
                return false;
            if (ccB.Mode == CameraMode::Overlay && ccA.Mode != CameraMode::Overlay)
                return true;
            return ccA.Priority < ccB.Priority;
        });
        for (entt::entity camera : cameras)
        {
            CameraData data;
            auto [transform, cameraComponent, enabled] = m_Registry.get<TransformComponent, CameraComponent, EnabledFlag>(camera);
            data.Frustum = cameraComponent.Frustum;
            data.ViewMatrix = transform.GetInverseMatrix();
            data.Viewport = cameraComponent.Viewport;
            data.ClippingPlanes = cameraComponent.ClippingPlanes;
            data.ClearColor = cameraComponent.ClearColor;
            data.Mode = cameraComponent.Mode;

            for (auto entity : m_Registry.view<AnimatorComponent, EnabledFlag>())
            {
                auto& animation = m_Registry.get<AnimatorComponent>(entity);
                animation.OnUpdate(ts);
                if (m_Registry.has<ModelRendererComponent>(entity))
                {
                    auto& model = m_Registry.get<ModelRendererComponent>(entity);
                    for (auto& model : model.Model->GetSubModels())
                    {
                        if (model.Mesh->IsAnimated())
                        {
                            const Ref<AnimatedMesh>& animatedMesh = (const Ref<AnimatedMesh>&)model.Mesh;
                            if (animatedMesh->IsCompatible(animation.GetCurrentAnimation()))
                            {
                                animation.Apply(animatedMesh);
                            }
                        }
                    }
                }
            }

            s_LightSources.clear();
            s_LightSourceShadowLayerMasks.clear();
            for (auto entity : m_Registry.view<TransformComponent, LightSourceComponent, EnabledFlag>())
            {
                if (CheckLayerMask(entity, cameraComponent.LayerMask))
                {
                    auto [transform, light] = m_Registry.get<TransformComponent, LightSourceComponent>(entity);
                    LightSource source;
                    source.Position = transform.GetPosition();
                    source.Direction = transform.GetForward();
                    source.Ambient = light.Ambient;
                    source.Color = light.Color;
                    source.Attenuation = light.Attenuation;
                    source.Intensity = light.Intensity;
                    source.Type = light.Type;
                    source.ShadowFramebuffer = light.Shadows.Enabled ? light.Shadows.RenderTarget : nullptr;
                    s_LightSources.push_back(source);
                    s_LightSourceShadowLayerMasks.push_back(light.Shadows.LayerMask);
                }
            }

            Ref<Framebuffer> framebuffer = cameraComponent.RenderTarget ? cameraComponent.RenderTarget : m_DefaultFramebuffer;

            m_Renderer->SetTime(m_Time);
            m_Renderer->BeginScene(framebuffer, data, s_LightSources);
            for (auto entity : m_Registry.view<TransformComponent, ModelRendererComponent, EnabledFlag>())
            {
                if (CheckLayerMask(entity, cameraComponent.LayerMask))
                {
                    auto [transform, model] = m_Registry.get<TransformComponent, ModelRendererComponent>(entity);
                    RenderOptions options;
                    options.ShadowMask = 0;
                    for (int i = 0; i < s_LightSourceShadowLayerMasks.size(); i++)
                        options.ShadowMask.set(i, CheckLayerMask(entity, s_LightSourceShadowLayerMasks[i]));
                    m_Renderer->RenderModel(model.Model, transform.GetMatrix(), options);
                }
            }
            m_Renderer->EndScene();
        }
    }

    void Scene::FindPrimaryCamera()
    {
        if (m_PrimaryCamera == entt::null || !m_Registry.has<CameraComponent>(m_PrimaryCamera))
        {
            auto view = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto entity : view)
            {
                m_PrimaryCamera = entity;
                break;
            }
        }
    }

    bool Scene::CheckLayerMask(entt::entity entity, uint64_t layerMask) const
    {
        return layerMask & m_Registry.get<LayerId>(entity).Mask;
    }

    glm::mat4 Scene::GenerateProjViewMatrixForLight(const LightSource& light) const
    {
        return glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 500.0f) * glm::lookAt(light.Position, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
    }

}

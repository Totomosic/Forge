#include "ForgePch.h"
#include "Scene.h"

#include "EntityUtils.h"
#include "Entity.h"
#include "Transform.h"
#include "CameraComponent.h"
#include "ModelRenderer.h"
#include "AnimatorComponent.h"
#include "Components.h"
#include "SpriteRenderer.h"
#include "Colliders.h"

#include "Assets/GraphicsCache.h"

namespace Forge
{

    template<typename T>
    void Clone(Entity& to, Entity from)
    {
        if (from.HasComponent<T>())
            to.AddComponent<T>(CloneComponent(from.GetComponent<T>()));
    }

    Scene::Scene(const Ref<Framebuffer>& defaultFramebuffer, Renderer3D* renderer)
        : m_Registry(),
          m_PrimaryCamera(entt::null),
          m_Time(0.0f),
          m_Renderer(renderer),
          m_Renderer2D(nullptr),
          m_DefaultFramebuffer(defaultFramebuffer),
          m_PickFramebuffer(nullptr),
          m_DebugDrawColliders(false)
    {
        if (m_Renderer)
            m_Renderer2D = std::make_unique<Renderer2D>();
    }

    Entity Scene::GetPrimaryCamera()
    {
        FindPrimaryCamera();
        return Entity(m_PrimaryCamera, &m_Registry);
    }

    Entity Scene::CreateEntity(uint8_t layer)
    {
        return CreateEntity("<Unnamed>", layer);
    }

    Entity Scene::CreateEntity(const std::string& name, uint8_t layer)
    {
        Entity entity(m_Registry.create(), &m_Registry);
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<LayerId>(1ULL << layer);
        entity.AddComponent<TagComponent>(name);
        entity.SetEnabled(true);
        return entity;
    }

    Entity Scene::CloneEntity(Entity entity)
    {
        Entity result(m_Registry.create(), &m_Registry);
        Clone<TransformComponent>(result, entity);
        Clone<LayerId>(result, entity);
        Clone<TagComponent>(result, entity);
        Clone<EnabledFlag>(result, entity);
        Clone<ModelRendererComponent>(result, entity);
        Clone<CameraComponent>(result, entity);
        Clone<PointLightComponent>(result, entity);
        Clone<DirectionalLightComponent>(result, entity);
        return result;
    }

    void Scene::DestroyEntity(entt::entity entity, bool destroyChildren)
    {
        if (destroyChildren)
        {
            Entities::DestroyChilden(entity, m_Registry);
        }
        Entities::SetParent(entity, entt::null, false, m_Registry);
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
        entity.GetComponent<LayerId>().Mask = FULL_LAYER_MASK;
    }

    void Scene::SetPrimaryCamera(const Entity& entity)
    {
        FORGE_ASSERT(entity.HasComponent<CameraComponent>(), "Camera must have a CameraComponent");
        m_PrimaryCamera = entity;
    }

    void Scene::Clear()
    {
        m_Registry.clear();
        m_PrimaryCamera = entt::null;
    }

    Entity Scene::NullEntity()
    {
        return Entity(entt::null, &m_Registry);
    }

    Entity Scene::CreateCamera(const Frustum& frustum)
    {
        Entity camera = CreateEntity("Camera");
        camera.AddComponent<CameraComponent>(frustum).Viewport = {
          0, 0, m_DefaultFramebuffer->GetWidth(), m_DefaultFramebuffer->GetHeight()};
        return camera;
    }

    PickResult Scene::PickEntity(const glm::vec2& viewportCoord, const Entity& camera, PickOptions options)
    {
        if (!m_Renderer)
        {
            PickResult result;
            result.Entity = NullEntity();
            return result;
        }

        CameraData data;
        auto [transform, cameraComponent] = m_Registry.get<TransformComponent, CameraComponent>(camera);
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
            props.Attachments = {FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth};
            m_PickFramebuffer = Framebuffer::Create(props);
        }
        else if (m_PickFramebuffer->GetWidth() != data.Viewport.Width ||
                 m_PickFramebuffer->GetHeight() != data.Viewport.Height)
        {
            m_PickFramebuffer->SetSize(data.Viewport.Width, data.Viewport.Height);
        }

        m_PickFramebuffer->Bind();
        RenderCommand::ClearDepth();
        m_PickFramebuffer->ClearAttachment(0, -1);
        m_Renderer->BeginPickScene(m_PickFramebuffer, data);
        for (auto entity : m_Registry.view<TransformComponent, ModelRendererComponent, EnabledFlag>())
        {
            if (CheckLayerMask(entity, cameraComponent.LayerMask & options.LayerMask))
            {
                auto [transform, model] = m_Registry.get<TransformComponent, ModelRendererComponent>(entity);
                RenderOptions options;
                options.EntityId = (int)entity;
                m_Renderer->RenderModel(model.Model, transform.GetMatrix(), options);
            }
        }
        m_Renderer->EndScene();

        int pixel = m_PickFramebuffer->ReadPixel(0, viewportCoord.x, viewportCoord.y);

        PickResult result;
        result.Entity = Entity((entt::entity)pixel, &m_Registry);

        if (options.IncludeCoordinate && result.Entity)
        {
            float depth = m_PickFramebuffer->ReadDepthPixel(viewportCoord.x, viewportCoord.y);
            glm::vec4 position = {viewportCoord.x / data.Viewport.Width * 2.0f - 1.0f,
              viewportCoord.y / data.Viewport.Height * 2.0f - 1.0f,
              depth * 2.0f - 1.0f,
              1.0f};
            glm::vec4 eyePos = glm::inverse(data.Frustum.ProjectionMatrix) * position;
            glm::vec4 worldPos = transform.GetMatrix() * eyePos;
            worldPos.x /= worldPos.w;
            worldPos.y /= worldPos.w;
            worldPos.z /= worldPos.w;
            result.Coordinate = {worldPos.x, worldPos.y, worldPos.z};
        }

        return result;
    }

    void Scene::OnUpdate(Timestep ts)
    {
        for (const auto& system : m_Systems)
            system(m_Registry, ts);

        static std::vector<LightSource> s_LightSources;
        static std::vector<LayerMask> s_LightSourceShadowLayerMasks;

        m_Time += ts.Seconds();

        if (m_Renderer)
        {
            auto cameraView = m_Registry.view<TransformComponent, CameraComponent, EnabledFlag>();
            std::vector<entt::entity> cameras = {cameraView.begin(), cameraView.end()};
            std::sort(cameras.begin(),
              cameras.end(),
              [this](entt::entity a, entt::entity b)
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
                auto [transform, cameraComponent, enabled] =
                  m_Registry.get<TransformComponent, CameraComponent, EnabledFlag>(camera);
                data.Frustum = cameraComponent.Frustum;
                data.ViewMatrix = transform.GetInverseMatrix();
                data.Viewport = cameraComponent.Viewport;
                data.ClippingPlanes = cameraComponent.ClippingPlanes;
                data.ClearColor = cameraComponent.ClearColor;
                data.Mode = cameraComponent.Mode;
                data.UsePostProcessing = cameraComponent.UsePostProcessing;

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
                for (auto entity : m_Registry.view<TransformComponent, PointLightComponent, EnabledFlag>())
                {
                    if (CheckLayerMask(entity, cameraComponent.LayerMask))
                    {
                        auto [transform, light] = m_Registry.get<TransformComponent, PointLightComponent>(entity);
                        LightSource source;
                        source.Position = transform.GetPosition();
                        source.Direction = transform.GetForward();
                        source.Ambient = light.Ambient;
                        source.Color = light.Color;
                        source.Attenuation = {1, 0.0f, 1.0f / (light.Radius * light.Radius)};
                        source.Intensity = light.Intensity;
                        source.Type = light.Type;
                        source.ShadowFramebuffer = light.Shadows.Enabled ? light.Shadows.RenderTarget : nullptr;
                        source.ShadowFrustum = cameraComponent.Frustum;
                        s_LightSources.push_back(source);
                        s_LightSourceShadowLayerMasks.push_back(light.Shadows.LayerMask);
                    }
                }
                for (auto entity : m_Registry.view<TransformComponent, DirectionalLightComponent, EnabledFlag>())
                {
                    if (CheckLayerMask(entity, cameraComponent.LayerMask))
                    {
                        auto [transform, light] = m_Registry.get<TransformComponent, DirectionalLightComponent>(entity);
                        LightSource source;
                        source.Position = transform.GetPosition();
                        source.Direction = transform.GetForward();
                        source.Ambient = light.Ambient;
                        source.Color = light.Color;
                        source.Attenuation = {1, 0, 0};
                        source.Intensity = light.Intensity;
                        source.Type = light.Type;
                        source.ShadowFramebuffer = light.Shadows.Enabled ? light.Shadows.RenderTarget : nullptr;
                        source.ShadowFrustum = Frustum::Orthographic(-25, 25, -25, 25, -20, 20);
                        s_LightSources.push_back(source);
                        s_LightSourceShadowLayerMasks.push_back(light.Shadows.LayerMask);
                    }
                }

                Ref<Framebuffer> framebuffer =
                  cameraComponent.RenderTarget ? cameraComponent.RenderTarget : m_DefaultFramebuffer;

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
                        if (m_DebugDrawColliders && m_Registry.has<AabbColliderComponent>(entity))
                        {
                            AabbColliderComponent& collider = m_Registry.get<AabbColliderComponent>(entity);
                            Ref<Material> material = GraphicsCache::DefaultColorMaterial(COLOR_GREEN);
                            material->GetSettings().Mode = PolygonMode::Line;
                            Ref<Model> model = Model::Create(GraphicsCache::CubeMesh(), material);
                            model->GetSubModels()[0].Transform = glm::scale(glm::mat4(1.0f), collider.Dimensions);
                            m_Renderer->RenderModel(model, transform.GetMatrix() * collider.Transform, options);
                        }
                    }
                }

                m_Renderer2D->BeginScene();
                for (auto entity : m_Registry.view<TransformComponent, SpriteRendererComponent, EnabledFlag>())
                {
                    if (CheckLayerMask(entity, cameraComponent.LayerMask))
                    {
                        auto [transform, sprite] = m_Registry.get<TransformComponent, SpriteRendererComponent>(entity);
                        m_Renderer2D->DrawQuad(
                          transform.GetPosition(), transform.GetScale(), sprite.Texture, sprite.Color);
                    }
                }
                m_Renderer2D->EndScene();
                const Ref<Model>* renderables = m_Renderer2D->GetRenderables();
                for (uint32_t i = 0; i < m_Renderer2D->GetRenderableCount(); i++)
                    m_Renderer->RenderModel(renderables[i], glm::mat4(1.0f));

                m_Renderer->EndScene();
            }
        }
    }

    void Scene::FindPrimaryCamera()
    {
        if (m_Registry.valid(m_PrimaryCamera) || !m_Registry.has<CameraComponent>(m_PrimaryCamera))
        {
            auto view = m_Registry.view<TransformComponent, CameraComponent, EnabledFlag>();
            for (auto entity : view)
            {
                m_PrimaryCamera = entity;
                break;
            }
        }
    }

    bool Scene::CheckLayerMask(entt::entity entity, LayerMask layerMask) const
    {
        return layerMask & m_Registry.get<LayerId>(entity).Mask;
    }

    glm::mat4 Scene::GenerateProjViewMatrixForLight(const LightSource& light) const
    {
        return glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 500.0f) *
               glm::lookAt(light.Position, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    }

}

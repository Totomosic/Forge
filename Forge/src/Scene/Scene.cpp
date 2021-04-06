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

    Scene::Scene(const Ref<Framebuffer>& defaultFramebuffer)
        : m_Registry(), m_PrimaryCamera(entt::null), m_DefaultFramebuffer(defaultFramebuffer)
    {
    }

    Entity Scene::GetPrimaryCamera()
    {
        FindPrimaryCamera();
        return Entity(m_PrimaryCamera, this);
    }

    Entity Scene::CreateEntity(uint8_t layer)
    {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<LayerId>(1ULL << layer);
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

    Entity Scene::CreateCamera(const glm::mat4& projection)
    {
        Entity camera = CreateEntity();
        camera.AddComponent<CameraComponent>(projection).Viewport = { 0, 0, m_DefaultFramebuffer->GetWidth(), m_DefaultFramebuffer->GetHeight() };
        return camera;
    }

    void Scene::OnUpdate(Timestep ts, Renderer3D& renderer)
    {
        auto cameraView = m_Registry.view<TransformComponent, CameraComponent>();
        std::vector<entt::entity> cameras = { cameraView.begin(), cameraView.end() };
        std::sort(cameras.begin(), cameras.end(), [this](entt::entity a, entt::entity b)
        {
            CameraComponent& ccA = m_Registry.get<CameraComponent>(a);
            CameraComponent& ccB = m_Registry.get<CameraComponent>(b);
            if (ccA.RenderTarget == nullptr && ccB.RenderTarget != nullptr)
                return false;
            if (ccB.RenderTarget == nullptr && ccA.RenderTarget != nullptr)
                return true;
            return ccA.Priority <= ccB.Priority;
        });
        for (entt::entity camera : cameras)
        {
            CameraData data;
            auto [transform, cameraComponent] = m_Registry.get<TransformComponent, CameraComponent>(camera);
            data.ProjectionMatrix = cameraComponent.ProjectionMatrix;
            data.ViewMatrix = transform.GetInverseMatrix();
            data.Viewport = cameraComponent.Viewport;
            data.ClippingPlanes = cameraComponent.ClippingPlanes;
            data.ClearColor = cameraComponent.ClearColor;

            for (auto entity : m_Registry.view<AnimatorComponent>())
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

            std::vector<LightSource> lightSources;
            for (auto entity : m_Registry.view<TransformComponent, LightSourceComponent>())
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
                    source.Type = light.Type;
                    lightSources.push_back(source);
                }
            }

            Ref<Framebuffer> framebuffer = cameraComponent.RenderTarget ? cameraComponent.RenderTarget : m_DefaultFramebuffer;

            renderer.BeginScene(framebuffer, data, lightSources);
            for (auto entity : m_Registry.view<TransformComponent, ModelRendererComponent>())
            {
                if (CheckLayerMask(entity, cameraComponent.LayerMask))
                {
                    auto [transform, model] = m_Registry.get<TransformComponent, ModelRendererComponent>(entity);
                    renderer.RenderModel(model.Model, transform.GetMatrix());
                }
            }
            renderer.EndScene();
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

}

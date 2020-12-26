#include "ForgePch.h"
#include "Scene.h"

#include "Entity.h"
#include "Transform.h"
#include "CameraComponent.h"
#include "ModelRenderer.h"
#include "Components.h"

namespace Forge
{

    Scene::Scene()
        : m_Registry(), m_PrimaryCamera(entt::null)
    {
    }

    Entity Scene::GetPrimaryCamera()
    {
        FindPrimaryCamera();
        return Entity(m_PrimaryCamera, this);
    }

    Entity Scene::CreateEntity()
    {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<TransformComponent>();
        return entity;
    }

    void Scene::DestroyEntity(const Entity& entity)
    {
        m_Registry.destroy(entity);
    }

    void Scene::SetPrimaryCamera(const Entity& entity)
    {
        FORGE_ASSERT(entity.HasComponent<CameraComponent>(), "Camera must have a CameraComponent");
        m_PrimaryCamera = entity;
    }

    void Scene::OnUpdate(Timestep ts, Renderer3D& renderer)
    {
        if (HasPrimaryCamera())
        {
            CameraData data;
            Entity camera = GetPrimaryCamera();
            auto [transform, cameraComponent] = m_Registry.get<TransformComponent, CameraComponent>(camera);
            data.ProjectionMatrix = cameraComponent.ProjectionMatrix;
            data.ViewMatrix = transform.GetInverseMatrix();

            std::vector<LightSource> lightSources;
            for (auto entity : m_Registry.view<TransformComponent, LightSourceComponent>())
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

            renderer.BeginScene(data, lightSources);
            for (auto entity : m_Registry.view<TransformComponent, ModelRendererComponent>())
            {
                auto [transform, model] = m_Registry.get<TransformComponent, ModelRendererComponent>(entity);
                renderer.RenderModel(model.Model, transform.GetMatrix());
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

}

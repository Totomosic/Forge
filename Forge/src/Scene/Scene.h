#pragma once
#include "Constants.h"
#include "Core/Timestep.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/Renderer2D.h"

#include <entt/entt.hpp>
#include <map>

namespace Forge
{

    namespace Detail
    {

        // C++17 fold expression https://en.cppreference.com/w/cpp/language/fold
        template<typename... Args>
        inline constexpr LayerMask CreateLayerMask(Args... args)
        {
            return ((LayerMask(1) << args) | ...);
        }

    }

#define FORGE_LAYERS(...) ::Forge::Detail::CreateLayerMask(__VA_ARGS__)

    class Entity;
    struct PickResult;

    struct FORGE_API PickOptions
    {
    public:
        bool IncludeCoordinate = false;
        LayerMask LayerMask = FULL_LAYER_MASK;
    };

    class FORGE_API Scene
    {
    private:
        static constexpr uint8_t DEFAULT_LAYER = 0;

        entt::registry m_Registry;
        entt::entity m_PrimaryCamera;
        float m_Time;

        Renderer3D* m_Renderer;
        Renderer2D m_Renderer2D;
        Ref<Framebuffer> m_DefaultFramebuffer;
        Ref<Framebuffer> m_PickFramebuffer;

        bool m_DebugDrawColliders;

    public:
        Scene(const Ref<Framebuffer>& defaultFramebuffer, Renderer3D* renderer);

        inline const Ref<Framebuffer>& GetPickFramebuffer() const
        {
            return m_PickFramebuffer;
        }

        inline bool HasPrimaryCamera()
        {
            FindPrimaryCamera();
            return m_PrimaryCamera != entt::null;
        }
        Entity GetPrimaryCamera();
        Entity CreateEntity(uint8_t layer = DEFAULT_LAYER);
        Entity CreateEntity(const std::string& name, uint8_t layer = DEFAULT_LAYER);
        Entity CloneEntity(Entity entity);
        void DestroyEntity(const Entity& entity);
        void SetLayer(Entity entity, uint8_t layer);
        void AddLayer(Entity entity, uint8_t layer);
        void RemoveLayer(Entity entity, uint8_t layer);
        void AddToAllLayers(Entity entity);
        void Clear();

        inline bool GetDebugRenderColliders() const
        {
            return m_DebugDrawColliders;
        }
        inline void SetDebugRenderColliders(bool render)
        {
            m_DebugDrawColliders = render;
        }

        template<typename T>
        Entity GetComponentOwner(const T& component)
        {
            return Entity(entt::to_entity(m_Registry, component), this);
        }

        inline entt::registry& GetRegistry()
        {
            return m_Registry;
        }

        void SetPrimaryCamera(const Entity& entity);
        Entity CreateCamera(const Frustum& frustum);

        PickResult PickEntity(const glm::vec2& viewportCoord, const Entity& camera, PickOptions options = {});

        void OnUpdate(Timestep ts);

        friend class Entity;

    private:
        void FindPrimaryCamera();
        bool CheckLayerMask(entt::entity entity, LayerMask layerMask) const;
        glm::mat4 GenerateProjViewMatrixForLight(const LightSource& light) const;
    };

}

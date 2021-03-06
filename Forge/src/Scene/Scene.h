#pragma once
#include "Constants.h"
#include "Core/Timestep.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/Renderer2D.h"
#include "Entity.h"

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

    struct PickResult;

    struct FORGE_API PickOptions
    {
    public:
        bool IncludeCoordinate = false;
        Forge::LayerMask LayerMask = FULL_LAYER_MASK;
    };

    class FORGE_API Scene
    {
    public:
        using System = std::function<void(entt::registry&, Timestep)>;

    private:
        static constexpr uint8_t DEFAULT_LAYER = 0;

        entt::registry m_Registry;
        entt::entity m_PrimaryCamera;
        float m_Time;

        Renderer3D* m_Renderer;
        std::unique_ptr<Renderer2D> m_Renderer2D;
        Ref<Framebuffer> m_DefaultFramebuffer;
        Ref<Framebuffer> m_PickFramebuffer;

        std::vector<System> m_Systems;

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
        // Be careful when destroying children entities, it is valid to destroy entities
        // while iterating over them so long as only the entities from the view are being
        // destroyed (and therefore, not their children)
        // https://github.com/skypjack/entt/issues/98
        void DestroyEntity(entt::entity entity, bool destroyChildren = true);
        void SetLayer(Entity entity, uint8_t layer);
        void AddLayer(Entity entity, uint8_t layer);
        void RemoveLayer(Entity entity, uint8_t layer);
        void AddToAllLayers(Entity entity);
        void Clear();

        Entity NullEntity();

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

        template<typename T>
        void AddSystem(const T& system)
        {
            m_Systems.push_back(system);
        }

        void OnUpdate(Timestep ts);

    private:
        void FindPrimaryCamera();
        bool CheckLayerMask(entt::entity entity, LayerMask layerMask) const;
        glm::mat4 GenerateProjViewMatrixForLight(const LightSource& light) const;
    };

}

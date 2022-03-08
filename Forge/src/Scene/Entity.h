#pragma once
#include "ForgePch.h"
#include "Scene.h"
#include "Transform.h"
#include "Components.h"
#include "EntityUtils.h"

#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace Forge
{

    class FORGE_API Entity
    {
    private:
        entt::entity m_Handle = entt::null;
        Scene* m_Scene = nullptr;

    public:
        Entity() = default;
        inline Entity(entt::entity handle, Scene* scene) : m_Handle(handle), m_Scene(scene) {}

        inline operator bool() const
        {
            return m_Handle != entt::null && m_Scene != nullptr;
        }
        inline operator entt::entity() const
        {
            return m_Handle;
        }
        inline operator uint32_t() const
        {
            return uint32_t(m_Handle);
        }

        inline bool Enabled() const
        {
            return m_Scene->m_Registry.has<EnabledFlag>(m_Handle);
        }

        inline void SetEnabled(bool enabled)
        {
            if (enabled != Enabled())
            {
                if (enabled)
                    AddComponent<EnabledFlag>();
                else
                    RemoveComponent<EnabledFlag>();
            }
        }

        inline std::string GetTag() const
        {
            TagComponent* tag = m_Scene->m_Registry.try_get<TagComponent>(m_Handle);
            if (tag)
                return tag->Tag;
            return "";
        }

        inline void SetTag(const std::string& tag)
        {
            TagComponent* t = m_Scene->m_Registry.try_get<TagComponent>(m_Handle);
            if (t)
                t->Tag = tag;
            else
                AddComponent<TagComponent>(TagComponent {tag});
        }

        inline Entity GetParent()
        {
            ParentComponent* parent = TryGetComponent<ParentComponent>();
            if (parent)
            {
                return Entity(parent->Parent, GetScene());
            }
            return GetScene()->NullEntity();
        }

        inline Entity GetRootElement()
        {
            Entity current = *this;
            Entity parent = current.GetParent();
            while (parent)
            {
                current = parent;
                parent = current.GetParent();
            }
            return current;
        }

        template<typename T>
        bool HasComponent() const
        {
            return m_Scene->m_Registry.has<T>(m_Handle);
        }

        template<typename T>
        T* TryGetComponent()
        {
            return m_Scene->m_Registry.try_get<T>(m_Handle);
        }

        template<typename T>
        T& GetComponent()
        {
            FORGE_ASSERT(HasComponent<T>(), "Component does not exist");
            return m_Scene->m_Registry.get<T>(m_Handle);
        }

        template<typename T, typename... Args>
        std::tuple<T&, Args&...> GetComponents()
        {
            return m_Scene->m_Registry.get<T, Args...>(m_Handle);
        }

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            FORGE_ASSERT(!HasComponent<T>(), "Component already exists");
            return m_Scene->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
        }

        template<typename T>
        void RemoveComponent()
        {
            FORGE_ASSERT(HasComponent<T>(), "Component does not exist");
            m_Scene->m_Registry.remove<T>(m_Handle);
        }

        inline Entity FindChild(const std::function<bool(Entity)>& predicate)
        {
            for (entt::entity child : Entities::GetChildren(m_Handle, GetRegistry()))
            {
                Entity ent(child, m_Scene);
                if (predicate(ent))
                    return ent;
            }
            return m_Scene->NullEntity();
        }

        inline entt::registry& GetRegistry() const
        {
            return m_Scene->GetRegistry();
        }

        inline Scene* GetScene() const
        {
            return m_Scene;
        }

        inline TransformComponent& GetTransform()
        {
            return GetComponent<TransformComponent>();
        }

        inline bool operator==(const Entity& other) const
        {
            return m_Handle == other.m_Handle && m_Scene == other.m_Scene;
        }
        inline bool operator!=(const Entity& other) const
        {
            return !(*this == other);
        }
    };

    struct FORGE_API PickResult
    {
    public:
        Forge::Entity Entity;
        glm::vec3 Coordinate;
    };

}

#pragma once
#include "Components.h"
#include "Transform.h"

namespace Forge
{
    namespace Entities
    {

        namespace Detail
        {

            inline void AddChild(entt::entity child, entt::entity parent, entt::registry& registry)
            {
                FORGE_ASSERT(child != parent, "Cannot add child to itself");
                ParentComponent p;
                p.NextChild = entt::null;
                p.PreviousChild = entt::null;
                p.Parent = parent;

                ChildrenComponent* children = registry.try_get<ChildrenComponent>(parent);
                if (!children)
                {
                    ChildrenComponent c;
                    c.ChildCount = 1;
                    c.FirstChild = child;
                    registry.emplace<ChildrenComponent>(parent, c);
                }
                else
                {
                    registry.get<ParentComponent>(children->FirstChild).PreviousChild = child;
                    p.NextChild = children->FirstChild;
                    children->ChildCount++;
                    children->FirstChild = child;
                }
                registry.emplace<ParentComponent>(child, p);
            }

            inline void RemoveChild(entt::entity child, entt::entity parent, entt::registry& registry)
            {
                FORGE_ASSERT(child != parent, "Cannot remove child from itself");
                ParentComponent& p = registry.get<ParentComponent>(child);
                if (registry.valid(p.PreviousChild))
                {
                    registry.get<ParentComponent>(p.PreviousChild).NextChild = p.NextChild;
                }
                if (registry.valid(p.NextChild))
                {
                    registry.get<ParentComponent>(p.NextChild).PreviousChild = p.PreviousChild;
                }
                ChildrenComponent& children = registry.get<ChildrenComponent>(parent);
                children.ChildCount--;
                if (children.ChildCount <= 0)
                {
                    registry.remove<ChildrenComponent>(parent);
                }
                registry.remove<ParentComponent>(child);
            }

        }

        class FORGE_API ChildIterator
        {
        private:
            entt::registry* m_Registry;
            entt::entity m_Entity;
            entt::entity m_Next;

        public:
            inline ChildIterator(entt::registry* registry, entt::entity entity)
                : m_Registry(registry), m_Entity(entity), m_Next(entt::null)
            {
                if (m_Registry->valid(m_Entity))
                {
                    m_Next = m_Registry->get<ParentComponent>(m_Entity).NextChild;
                }
            }

            inline entt::entity operator*() const
            {
                return m_Entity;
            }

            inline entt::entity operator->() const
            {
                return m_Entity;
            }

            inline ChildIterator operator++(int) const
            {
                return ChildIterator(m_Registry, m_Next);
            }

            inline ChildIterator& operator++()
            {
                m_Entity = m_Next;
                return *this;
            }

            inline ChildIterator operator--(int) const
            {
                return ChildIterator(m_Registry, m_Registry->get<ParentComponent>(m_Entity).PreviousChild);
            }

            inline ChildIterator& operator--()
            {
                m_Entity = m_Registry->get<ParentComponent>(m_Entity).PreviousChild;
                return *this;
            }

            inline bool operator==(const ChildIterator& other) const
            {
                return m_Entity == other.m_Entity;
            }

            inline bool operator!=(const ChildIterator& other) const
            {
                return !(*this == other);
            }
        };

        class FORGE_API ChildContainer
        {
        private:
            entt::registry* m_Registry;
            entt::entity m_Entity;

        public:
            inline ChildContainer(entt::registry* registry, entt::entity entity)
                : m_Registry(registry), m_Entity(entity)
            {
            }

            inline ChildIterator begin() const
            {
                ChildrenComponent* children = m_Registry->try_get<ChildrenComponent>(m_Entity);
                if (children)
                {
                    return ChildIterator(m_Registry, children->FirstChild);
                }
                return end();
            }

            inline ChildIterator end() const
            {
                return ChildIterator(m_Registry, entt::null);
            }
        };

        inline void SetParent(entt::entity child, entt::entity parent, bool updateTransform, entt::registry& registry)
        {
            ParentComponent* currentParent = registry.try_get<ParentComponent>(child);
            if (currentParent)
            {
                Detail::RemoveChild(child, currentParent->Parent, registry);
            }
            if (registry.valid(parent))
            {
                Detail::AddChild(child, parent, registry);
            }
            if (updateTransform)
            {
                TransformComponent* parentTransform = nullptr;
                if (registry.valid(parent))
                {
                    parentTransform = &registry.get<TransformComponent>(parent);
                }
                registry.get<TransformComponent>(child).SetParent(parentTransform);
            }
        }

        inline entt::entity GetParent(entt::entity entity, entt::registry& registry)
        {
            ParentComponent* parent = registry.try_get<ParentComponent>(entity);
            if (parent)
            {
                return parent->Parent;
            }
            return entt::null;
        }

        inline entt::entity GetRootEntity(entt::entity entity, entt::registry& registry)
        {
            entt::entity current = entity;
            entt::entity parent = GetParent(current, registry);
            while (registry.valid(parent))
            {
                current = parent;
                parent = GetParent(current, registry);
            }
            return current;
        }

        inline ChildContainer GetChildren(entt::entity entity, entt::registry& registry)
        {
            return ChildContainer(&registry, entity);
        }

        // Be careful when destroying children entities, it is valid to destroy entities
        // while iterating over them so long as only the entities from the view are being
        // destroyed (and therefore, not their children)
        // https://github.com/skypjack/entt/issues/98
        inline void DestroyChilden(entt::entity entity, entt::registry& registry)
        {
            ChildrenComponent* children = registry.try_get<ChildrenComponent>(entity);
            if (children)
            {
                entt::entity current = children->FirstChild;
                while (registry.valid(current))
                {
                    DestroyChilden(current, registry);
                    entt::entity toDestroy = current;
                    current = registry.get<ParentComponent>(current).NextChild;
                    registry.destroy(toDestroy);
                }
            }
        }

    }
}

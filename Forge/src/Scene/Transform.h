#pragma once
#include "ForgePch.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Forge
{

    FORGE_API enum class Space
    {
        World,
        Local,
    };

    class FORGE_API TransformComponent
    {
    private:
        glm::vec3 m_Position;
        glm::quat m_Rotation;
        glm::vec3 m_Scale;

        mutable bool m_Dirty;
        mutable glm::mat4 m_CacheTransform;
        mutable glm::mat4 m_CacheInvTransform;

        bool m_Flip = false;

        mutable const TransformComponent* m_Parent;
        mutable std::vector<const TransformComponent*> m_Children;

    public:
        inline TransformComponent(const glm::vec3& position = {0, 0, 0},
          const glm::quat& rotation = {1.0f, 0.0f, 0.0f, 0.0f}, const glm::vec3& scale = {1.0f, 1.0f, 1.0f})
            : m_Position(position),
              m_Rotation(rotation),
              m_Scale(scale),
              m_Dirty(true),
              m_CacheTransform(),
              m_CacheInvTransform(),
              m_Parent(nullptr),
              m_Children()
        {
        }

        TransformComponent(const TransformComponent& other) = delete;
        TransformComponent& operator=(const TransformComponent& other) = delete;

        inline TransformComponent(TransformComponent&& other)
            : m_Position(other.m_Position),
              m_Rotation(other.m_Rotation),
              m_Scale(other.m_Scale),
              m_Dirty(other.m_Dirty),
              m_CacheTransform(std::move(other.m_CacheTransform)),
              m_CacheInvTransform(std::move(other.m_CacheInvTransform)),
              m_Parent(other.m_Parent),
              m_Children(std::move(other.m_Children))
        {
            if (&other != this)
            {
                m_Parent = nullptr;
                SetParent(other.m_Parent);
                other.SetParent(nullptr);
                other.m_Children = {};
                for (const TransformComponent* child : m_Children)
                    child->m_Parent = this;
            }
        }

        inline TransformComponent& operator=(TransformComponent&& other)
        {
            if (&other != this)
            {
                m_Position = other.m_Position;
                m_Rotation = other.m_Rotation;
                m_Scale = other.m_Scale;
                m_Dirty = other.m_Dirty;
                m_CacheTransform = std::move(other.m_CacheTransform);
                m_CacheInvTransform = std::move(other.m_CacheInvTransform);
                m_Children = std::move(other.m_Children);
                other.m_Children = {};
                SetParent(other.m_Parent);
                other.SetParent(nullptr);
                for (const TransformComponent* child : m_Children)
                    child->m_Parent = this;
            }
            return *this;
        }

        inline ~TransformComponent()
        {
            for (const TransformComponent* child : m_Children)
            {
                child->m_Parent = nullptr;
                child->SetDirty();
            }
            m_Children.clear();
            SetParent(nullptr);
        }

        inline bool HasParent() const
        {
            return m_Parent != nullptr;
        }
        inline const TransformComponent* GetParent() const
        {
            return m_Parent;
        }
        inline const std::vector<const TransformComponent*> GetChildren() const
        {
            return m_Children;
        }

        inline void SetParent(const TransformComponent* parent)
        {
            if (parent != m_Parent && parent != this)
            {
                if (m_Parent)
                {
                    auto it = std::find(m_Parent->m_Children.begin(), m_Parent->m_Children.end(), this);
                    if (it != m_Parent->m_Children.end())
                        m_Parent->m_Children.erase(it);
                }
                m_Parent = parent;
                if (m_Parent)
                {
                    m_Parent->m_Children.push_back(this);
                }
                SetDirty();
            }
        }

        inline const glm::vec3& GetLocalPosition() const
        {
            return m_Position;
        }
        inline const glm::quat& GetLocalRotation() const
        {
            return m_Rotation;
        }
        inline const glm::vec3& GetLocalScale() const
        {
            return m_Scale;
        }

        inline const glm::mat4& GetLocalMatrix() const
        {
            RecalculateMatrices();
            return m_CacheTransform;
        }
        inline const glm::mat4& GetLocalInverseMatrix() const
        {
            RecalculateMatrices();
            return m_CacheInvTransform;
        }

        inline glm::mat4 GetMatrix() const
        {
            RecalculateMatrices();
            return m_Parent ? m_Parent->GetMatrix() * m_CacheTransform : m_CacheTransform;
        }
        inline glm::mat4 GetInverseMatrix() const
        {
            RecalculateMatrices();
            return m_Parent ? m_CacheInvTransform * m_Parent->GetInverseMatrix() : m_CacheInvTransform;
        }

        inline glm::vec3 GetPosition() const
        {
            return GetParentMatrix() * glm::vec4 {GetLocalPosition(), 1.0f};
        }
        inline glm::quat GetRotation() const
        {
            return m_Parent ? m_Parent->GetRotation() * GetLocalRotation() : GetLocalRotation();
        }
        inline glm::vec3 GetScale() const
        {
            return m_Parent ? m_Parent->GetScale() * GetLocalScale() : GetLocalScale();
        }

        inline void SetLocalPosition(const glm::vec3& position)
        {
            m_Position = position;
            SetDirty();
        }

        inline void SetLocalRotation(const glm::quat& rotation)
        {
            m_Rotation = rotation;
            SetDirty();
        }

        inline void SetLocalScale(const glm::vec3& scale)
        {
            m_Scale = scale;
            SetDirty();
        }

        inline glm::vec3 GetLocalForward() const
        {
            return GetLocalRotation() * glm::vec3 {0, 0, -1.0f};
        }

        inline glm::vec3 GetLocalRight() const
        {
            return GetLocalRotation() * glm::vec3 {1.0f, 0, 0};
        }

        inline glm::vec3 GetLocalUp() const
        {
            return GetLocalRotation() * glm::vec3 {0, 1.0f, 0};
        }

        inline glm::vec3 GetForward() const
        {
            return GetRotation() * glm::vec3 {0, 0, -1.0f};
        }

        inline glm::vec3 GetRight() const
        {
            return GetRotation() * glm::vec3 {1.0f, 0, 0};
        }

        inline glm::vec3 GetUp() const
        {
            return GetRotation() * glm::vec3 {0, 1.0f, 0};
        }

        inline void Translate(const glm::vec3& translation)
        {
            SetLocalPosition(GetLocalPosition() + translation);
        }

        inline void Rotate(const glm::quat& rotation)
        {
            SetLocalRotation(GetLocalRotation() * rotation);
        }

        inline void Rotate(float angle, glm::vec3 axis, Space space = Space::Local)
        {
            if (space == Space::World)
                axis = glm::inverse(GetLocalRotation()) * axis;
            Rotate(glm::angleAxis(angle, axis));
        }

        inline void Scale(const glm::vec3& scale)
        {
            SetLocalScale(GetLocalScale() * scale);
        }

        inline void SetForwardUp(const glm::vec3& forward, const glm::vec3& up)
        {
            glm::vec3 right = glm::normalize(glm::cross(forward, up));
            glm::vec3 realUp = glm::normalize(glm::cross(right, forward));
            glm::mat3 rotation = glm::mat3(right, realUp, -glm::normalize(forward));
            SetLocalRotation(glm::quat_cast(rotation));
        }

        inline void FlipX()
        {
            m_Flip = true;
            SetDirty();
        }

        inline TransformComponent Clone() const
        {
            TransformComponent result;
            result.m_Position = m_Position;
            result.m_Rotation = m_Rotation;
            result.m_Scale = m_Scale;
            result.m_Dirty = m_Dirty;
            result.m_CacheTransform = m_CacheTransform;
            result.m_CacheInvTransform = m_CacheInvTransform;
            result.SetParent(m_Parent);
            return result;
        }

        inline void SetFromTransform(const TransformComponent& other)
        {
            m_Position = other.m_Position;
            m_Rotation = other.m_Rotation;
            m_Scale = other.m_Scale;
            m_Dirty = other.m_Dirty;
            m_CacheTransform = other.m_CacheTransform;
            m_CacheInvTransform = other.m_CacheInvTransform;
            SetParent(other.m_Parent);
        }

    private:
        inline glm::mat4 GetParentMatrix() const
        {
            if (m_Parent)
                return m_Parent->GetMatrix();
            return glm::mat4(1.0f);
        }

        inline void RecalculateMatrices() const
        {
            if (m_Dirty)
            {
                m_CacheTransform = glm::translate(glm::mat4(1.0f), m_Position) * GetRotationMatrix() *
                                   glm::scale(glm::mat4(1.0f), m_Scale);
                m_CacheInvTransform = glm::inverse(m_CacheTransform);
                m_Dirty = false;
            }
        }

        inline glm::mat4 GetRotationMatrix() const
        {
            glm::mat4 matrix = glm::toMat4(m_Rotation);
            if (m_Flip)
            {
                glm::mat4 flip = glm::scale(glm::mat4(1.0f), glm::vec3 {1.0f, -1.0f, 1.0f});
                matrix = flip * matrix * flip;
            }
            return matrix;
        }

        inline void SetDirty() const
        {
            m_Dirty = true;
            for (const TransformComponent* child : m_Children)
                child->SetDirty();
        }
    };

    inline TransformComponent CloneComponent(const TransformComponent& component)
    {
        return component.Clone();
    }

}

#pragma once
#include "ForgePch.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Forge
{

	class FORGE_API TransformComponent
	{
	private:
		glm::vec3 m_Position;
		glm::quat m_Rotation;
		glm::vec3 m_Scale;

		mutable bool m_Dirty;
		mutable glm::mat4 m_CacheTransform;
		mutable glm::mat4 m_CacheInvTransform;

	public:
		inline TransformComponent(const glm::vec3& position = { 0, 0, 0 }, const glm::quat& rotation = { 1.0f, 0.0f, 0.0f, 0.0f }, const glm::vec3& scale = { 1.0f, 1.0f, 1.0f })
			: m_Position(position), m_Rotation(rotation), m_Scale(scale), m_Dirty(true), m_CacheTransform(), m_CacheInvTransform()
		{}

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline const glm::quat& GetRotation() const { return m_Rotation; }
		inline const glm::vec3& GetScale() const { return m_Scale; }

		inline const glm::mat4& GetMatrix() const { RecalculateMatrices(); return m_CacheTransform; }
		inline const glm::mat4& GetInverseMatrix() const { RecalculateMatrices(); return m_CacheInvTransform; }

		inline void SetPosition(const glm::vec3& position)
		{
			m_Position = position;
			m_Dirty = true;
		}

		inline void SetRotation(const glm::quat& rotation)
		{
			m_Rotation = rotation;
			m_Dirty = true;
		}

		inline void SetScale(const glm::vec3& scale)
		{
			m_Scale = scale;
			m_Dirty = true;
		}

		inline glm::vec3 GetForward() const
		{
			return GetRotation() * glm::vec3{ 0, 0, 1.0f };
		}

		inline glm::vec3 GetRight() const
		{
			return GetRotation() * glm::vec3{ 1.0f, 0, 0 };
		}

		inline glm::vec3 GetUp() const
		{
			return GetRotation() * glm::vec3{ 0, 1.0f, 0 };
		}

		inline void Translate(const glm::vec3& translation)
		{
			SetPosition(GetPosition() + translation);
		}

		inline void Rotate(const glm::quat& rotation)
		{
			SetRotation(GetRotation() * rotation);
		}

		inline void Rotate(float angle, const glm::vec3& axis)
		{
			Rotate(glm::angleAxis(angle, axis));
		}

		inline void Scale(const glm::vec3& scale)
		{
			SetScale(GetScale() * scale);
		}

	private:
		inline void RecalculateMatrices() const
		{
			if (m_Dirty)
			{
				m_CacheTransform = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(m_Rotation) * glm::scale(glm::mat4(1.0f), m_Scale);
				m_CacheInvTransform = glm::inverse(m_CacheTransform);
				m_Dirty = false;
			}
		}

	};

}
#pragma once
#include "ForgePch.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Forge
{

	struct FORGE_API JointTransform
	{
	public:
		// Relative to parent joint
		glm::vec3 Translation;
		glm::quat Orientation;

	public:
		inline glm::mat4 GetLocalTransform() const
		{
			return glm::translate(glm::mat4(1.0f), Translation) * glm::toMat4(Orientation);
		}

	public:
		static JointTransform Interpolate(const JointTransform& a, const JointTransform& b, float t)
		{
			glm::vec3 translation = a.Translation + (b.Translation - a.Translation) * t;
			glm::quat orientation = glm::slerp(a.Orientation, b.Orientation, t);
			return { translation, orientation };
		}
	};

	struct FORGE_API Joint
	{
	public:
		glm::mat4 Transform;
		int Id;
		std::vector<Scope<Joint>> Children;
		glm::mat4 InverseBindTransform;
	};

}

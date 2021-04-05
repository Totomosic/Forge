#include "ForgePch.h"
#include "AnimatedMesh.h"

namespace Forge
{

	void AddTransform(const Joint* joint, std::vector<glm::mat4>& transforms)
	{
		transforms[joint->Id] = joint->Transform;
		for (const auto& child : joint->Children)
			AddTransform(child.get(), transforms);
	}

	std::vector<glm::mat4> AnimatedMesh::GetJointTransforms() const
	{
		std::vector<glm::mat4> result(GetJointCount());
		AddTransform(&GetRootJoint(), result);
		return result;
	}

	void AnimatedMesh::Apply(const Ref<Shader>& shader, const ShaderRequirements& requirements)
	{
		if (requirements.Animation)
		{
			std::vector<glm::mat4> jointTransforms = GetJointTransforms();
			for (size_t i = 0; i < jointTransforms.size(); i++)
			{
				std::string uniformName = std::string(JointTransformsBase) + "[" + std::to_string(i) + "]";
				shader->SetUniform(uniformName, jointTransforms[i]);
			}
		}
	}

	void RemoveAnimationFromJoint(Joint* joint)
	{
		joint->Transform = glm::mat4(1.0f);
		for (const auto& child : joint->Children)
			RemoveAnimationFromJoint(child.get());
	}

	void AnimatedMesh::RemoveAnimation()
	{
		RemoveAnimationFromJoint(&GetRootJoint());
	}

}

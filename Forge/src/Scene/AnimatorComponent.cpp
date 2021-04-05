#include "ForgePch.h"
#include "AnimatorComponent.h"

namespace Forge
{

	AnimatorComponent::AnimatorComponent()
		: m_CurrentAnimation(), m_CurrentTime(0.0f)
	{
	}

	void AnimatorComponent::OnUpdate(Timestep ts)
	{
		if (m_CurrentAnimation)
		{
			m_CurrentTime += ts.Seconds() * 0.5f;
			if (m_CurrentTime >= m_CurrentAnimation->GetLength())
				m_CurrentTime -= m_CurrentAnimation->GetLength();
		}
	}

	void AnimatorComponent::Apply(const Ref<AnimatedMesh>& mesh)
	{
		if (m_CurrentAnimation == nullptr)
		{
			ApplyIdentityToJoints(&mesh->GetRootJoint());
		}
		else
		{
			FORGE_ASSERT(m_CurrentAnimation->KeyFrames[0].Transforms.size() == mesh->GetJointCount(), "Invalid mesh");
			ApplyPoseToJoints(CalculateCurrentPose(), &mesh->GetRootJoint(), glm::mat4(1.0f));
		}
	}

	std::vector<glm::mat4> AnimatorComponent::CalculateCurrentPose() const
	{
		AnimationKeyFrame* prev;
		AnimationKeyFrame* next;
		FindPrevAndNextKeyframes(&prev, &next);
		FORGE_ASSERT(prev != nullptr && next != nullptr, "Invalid animation");
		float progression = CalculateProgressionBetween(*prev, *next);
		return InterpolatePoses(*prev, *next, progression);
	}

	void AnimatorComponent::ApplyIdentityToJoints(Joint* joint) const
	{
		joint->Transform = glm::mat4(1.0f);
		for (const auto& child : joint->Children)
			ApplyIdentityToJoints(child.get());
	}

	void AnimatorComponent::ApplyPoseToJoints(const std::vector<glm::mat4>& pose, Joint* joint, const glm::mat4& parentTransform) const
	{
		const glm::mat4& localTransform = pose[joint->Id];
		glm::mat4 transform = parentTransform * localTransform;
		for (const auto& child : joint->Children)
			ApplyPoseToJoints(pose, child.get(), transform);
		glm::mat4 modelTransform = transform * joint->InverseBindTransform;
		joint->Transform = modelTransform;
	}

	void AnimatorComponent::FindPrevAndNextKeyframes(AnimationKeyFrame** prev, AnimationKeyFrame** next) const
	{
#ifndef FORGE_DIST
		*prev = nullptr;
		*next = nullptr;
#endif
		for (int i = 0; i < m_CurrentAnimation->KeyFrames.size() - 1; i++)
		{
			if (m_CurrentTime >= m_CurrentAnimation->KeyFrames[i].TimeStamp)
			{
				*prev = &m_CurrentAnimation->KeyFrames[i];
				*next = &m_CurrentAnimation->KeyFrames[i + 1];
			}
		}
	}

	float AnimatorComponent::CalculateProgressionBetween(const AnimationKeyFrame& prev, const AnimationKeyFrame& next) const
	{
		float delta = m_CurrentTime - prev.TimeStamp;
		return delta / (next.TimeStamp - prev.TimeStamp);
	}

	std::vector<glm::mat4> AnimatorComponent::InterpolatePoses(const AnimationKeyFrame& prev, const AnimationKeyFrame& next, float progression) const
	{
		FORGE_ASSERT(prev.Transforms.size() == next.Transforms.size(), "Invalid");
		std::vector<glm::mat4> result(prev.Transforms.size());
		for (size_t i = 0; i < prev.Transforms.size(); i++)
		{
			result[i] = JointTransform::Interpolate(prev.Transforms[i], next.Transforms[i], progression).GetLocalTransform();
		}
		return result;
	}

}

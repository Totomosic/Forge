#pragma once
#include "Core/Timestep.h"
#include "Renderer/Animation/AnimatedMesh.h"
#include "Renderer/Animation/Animation.h"

namespace Forge
{

	class FORGE_API AnimatorComponent
	{
	private:
		Ref<Animation> m_CurrentAnimation;
		float m_CurrentTime;

	public:
		AnimatorComponent();

		inline const Ref<Animation>& GetCurrentAnimation() const { return m_CurrentAnimation; }
		inline float GetAnimationTime() const { return m_CurrentTime; }
		inline void SetAnimationTime(float time) { m_CurrentTime = time; }

		inline void SetCurrentAnimation(const Ref<Animation>& animation, float startTime = 0.0f)
		{
			FORGE_ASSERT(animation == nullptr || animation->KeyFrames.size() > 0, "Invalid animation");
			m_CurrentAnimation = animation;
			m_CurrentTime = startTime;
		}
		
		void OnUpdate(Timestep ts);
		void Apply(const Ref<AnimatedMesh>& mesh);

		std::vector<glm::mat4> CalculateCurrentPose() const;

	private:		
		void ApplyIdentityToJoints(Joint* joint) const;
		void ApplyPoseToJoints(const std::vector<glm::mat4>& pose, Joint* joint, const glm::mat4& parentTransform) const;
		void FindPrevAndNextKeyframes(AnimationKeyFrame** prev, AnimationKeyFrame** next) const;
		float CalculateProgressionBetween(const AnimationKeyFrame& prev, const AnimationKeyFrame& next) const;
		std::vector<glm::mat4> InterpolatePoses(const AnimationKeyFrame& prev, const AnimationKeyFrame& next, float progression) const;

	};

}

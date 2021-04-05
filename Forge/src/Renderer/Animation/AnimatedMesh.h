#pragma once
#include "../Mesh.h"
#include "Joint.h"
#include "Animation.h"

namespace Forge
{

	struct FORGE_API Skeleton
	{
	public:
		Scope<Joint> Root;
		int JointCount;

	public:
		Skeleton() = default;
		inline Skeleton(Scope<Joint>&& root, int jointCount)
			: Root(std::move(root)), JointCount(jointCount)
		{}
	};

	class FORGE_API AnimatedMesh : public Mesh
	{
	private:
		Ref<Skeleton> m_Skeleton;

	public:
		AnimatedMesh() = default;
		inline AnimatedMesh(const Ref<VertexArray>& vertices, const Ref<Skeleton>& skeleton) : Mesh(vertices),
			m_Skeleton(skeleton)
		{
		}

		inline const Joint& GetRootJoint() const { return *m_Skeleton->Root; }
		inline Joint& GetRootJoint() { return *m_Skeleton->Root; }
		inline int GetJointCount() const { return m_Skeleton->JointCount; }
		std::vector<glm::mat4> GetJointTransforms() const;
		inline virtual bool IsAnimated() const override { return true; }

		inline bool IsCompatible(const Ref<Animation>& animation) const { return animation == nullptr || animation->KeyFrames[0].Transforms.size() == GetJointCount(); }
		virtual void Apply(const Ref<Shader>& shader, const ShaderRequirements& requirements) override;

	private:
		void RemoveAnimation();

	};

}

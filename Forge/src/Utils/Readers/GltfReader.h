#pragma once
#include "Renderer/Model.h"
#include "Renderer/Animation/AnimatedMesh.h"

namespace Forge
{

	class FORGE_API GltfReader
	{
	private:
		std::vector<Ref<Mesh>> m_Meshes;
		std::unordered_map<std::string, Ref<Animation>> m_Animations;

	public:
		GltfReader(const std::string& filename);

		const std::vector<Ref<Mesh>>& GetMeshes() const;
		inline bool HasAnimation(const std::string& name) const { return m_Animations.find(name) != m_Animations.end(); }
		inline const Ref<Animation>& GetAnimation(const std::string& name) const { return m_Animations.at(name); }

	private:
		void ReadGltf(const std::string& filename);

	};

}

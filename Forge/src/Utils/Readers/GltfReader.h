#pragma once
#include "Renderer/Model.h"

namespace Forge
{

	class FORGE_API GltfReader
	{
	private:
		std::vector<Ref<Mesh>> m_Meshes;

	public:
		GltfReader(const std::string& filename);

		const std::vector<Ref<Mesh>>& GetMeshes() const;

	private:
		void ReadGltf(const std::string& filename);

	};

}

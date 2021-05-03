#pragma once
#include "Renderer/Model.h"
#include "Renderer/Animation/AnimatedMesh.h"

namespace Forge
{

	class FORGE_API ObjReader
	{
	private:
		Ref<Mesh> m_Mesh;

	public:
		ObjReader(const std::string& filename);

		inline const Ref<Mesh>& GetMesh() const { return m_Mesh; }

	private:
		void ReadObj(const std::string& filename);

	};

}

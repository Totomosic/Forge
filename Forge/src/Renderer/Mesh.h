#pragma once
#include "VertexArray.h"

namespace Forge
{

	class FORGE_API Mesh
	{
	private:
		Ref<VertexArray> m_Vertices;

	public:
		inline Mesh()
			: m_Vertices()
		{}

		inline Mesh(const Ref<VertexArray>& vertices)
			: m_Vertices(vertices)
		{}

		inline const Ref<VertexArray>& GetVertices() const { return m_Vertices; }
	};

}

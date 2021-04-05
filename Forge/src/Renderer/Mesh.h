#pragma once
#include "VertexArray.h"
#include "Shader.h"
#include "RendererContext.h"

namespace Forge
{

	class FORGE_API Mesh
	{
	private:
		Ref<VertexArray> m_Vertices;
		GLuint m_DrawMode;

	public:
		inline Mesh()
			: m_Vertices(), m_DrawMode(GL_TRIANGLES)
		{}

		inline Mesh(const Ref<VertexArray>& vertices)
			: m_Vertices(vertices), m_DrawMode(GL_TRIANGLES)
		{}

		virtual ~Mesh() = default;

		inline GLuint GetDrawMode() const { return m_DrawMode; }
		inline void SetDrawMode(GLuint mode) { m_DrawMode = mode; }
		inline const Ref<VertexArray>& GetVertices() const { return m_Vertices; }
		inline virtual bool IsAnimated() const { return false; }

		inline virtual void Apply(const Ref<Shader>& shader, const ShaderRequirements& requirements) {}
	};

}

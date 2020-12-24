#pragma once
#include "Core/Color.h"
#include "Shader.h"
#include "VertexArray.h"

namespace Forge
{

	namespace Detail
	{

		void OnGlError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

	}

	class FORGE_API RenderCommand
	{
	public:
		static void Init();
		static void SetClearColor(const Color& color);
		static void Clear();
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void DrawIndexed(const Ref<VertexArray>& vertexArray);
	};

}

#pragma once
#include "Core/Color.h"
#include "Core/Viewport.h"
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
	private:
		static int s_LastClipPlaneCount;

	public:
		static void Init();
		static void BindDefaultFramebuffer();
		static void SetClearColor(const Color& color);
		static void Clear();
		inline static void SetViewport(const Viewport& viewport) { SetViewport(viewport.Left, viewport.Bottom, viewport.Width, viewport.Height); }
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void EnableWireframe(bool enable);
		static void DrawIndexed(GLuint drawMode, const Ref<VertexArray>& vertexArray);
		static void EnableClippingPlanes(int count);
	};

}

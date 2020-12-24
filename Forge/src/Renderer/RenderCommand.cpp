#include "ForgePch.h"
#include "RenderCommand.h"

namespace Forge
{

	namespace Detail
	{
		void OnGlError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
		{
			switch (severity)
			{
			case GL_DEBUG_SEVERITY_HIGH: FORGE_FATAL(message); return;
			case GL_DEBUG_SEVERITY_MEDIUM: FORGE_ERROR(message); return;
			case GL_DEBUG_SEVERITY_LOW: FORGE_WARN(message); return;
			case GL_DEBUG_SEVERITY_NOTIFICATION: FORGE_INFO(message); return;
			}
		}
	}

	void RenderCommand::Init()
	{
#ifdef FORGE_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(Detail::OnGlError, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void RenderCommand::SetClearColor(const Color& color)
	{
		glClearColor(float(color.r) / 255.0f, float(color.g) / 255.0f, float(color.b) / 255.0f, float(color.a) / 255.0f);
	}

	void RenderCommand::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void RenderCommand::DrawIndexed(const Ref<VertexArray>& vertexArray)
	{
		size_t count = vertexArray->GetIndexBuffer()->GetCount();
		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

}

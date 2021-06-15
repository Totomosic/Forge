#include "ForgePch.h"
#include "RenderCommand.h"
#include "ShaderLibrary.h"

namespace Forge
{

	namespace Detail
	{
		void OnGlError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
		{
			switch (severity)
			{
			case GL_DEBUG_SEVERITY_HIGH: FORGE_ASSERT(false, message); return;
			case GL_DEBUG_SEVERITY_MEDIUM: FORGE_ERROR(message); return;
			case GL_DEBUG_SEVERITY_LOW: FORGE_WARN(message); return;
			case GL_DEBUG_SEVERITY_NOTIFICATION: FORGE_INFO(message); return;
			}
		}
	}

	int RenderCommand::s_LastClipPlaneCount = 0;

	void RenderCommand::Init()
	{
#ifndef FORGE_DIST
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(Detail::OnGlError, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void RenderCommand::BindDefaultFramebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderCommand::SetClearColor(const Color& color)
	{
		glClearColor(float(color.r), float(color.g), float(color.b), float(color.a));
	}

	void RenderCommand::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RenderCommand::ClearDepth()
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void RenderCommand::EnableCullFace(bool enabled)
	{
		if (enabled)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
	}

	void RenderCommand::SetCullFace(CullFace face)
	{
		if (face != CullFace::None)
		{
			glCullFace((GLenum)face);
		}
	}

	void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void RenderCommand::EnableWireframe(bool enable)
	{
		if (enable)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void RenderCommand::DrawIndexed(GLuint drawMode, const Ref<VertexArray>& vertexArray)
	{
		uint32_t count = vertexArray->GetIndexBuffer()->GetCount();
		vertexArray->Bind();
		glDrawElements(drawMode, count, vertexArray->GetIndexBuffer()->GetGlDataType(), nullptr);
	}

	void RenderCommand::EnableClippingPlanes(int count)
	{
		for (int i = s_LastClipPlaneCount; i < count; i++)
		{
			glEnable(GL_CLIP_DISTANCE0 + i);
		}
		for (int i = count; i < s_LastClipPlaneCount; i++)
		{
			glDisable(GL_CLIP_DISTANCE0 + i);
		}
		s_LastClipPlaneCount = count;
	}

}

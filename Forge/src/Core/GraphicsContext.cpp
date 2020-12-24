#include "ForgePch.h"
#include "GraphicsContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Renderer/GraphicsCache.h"

namespace Forge
{

	GraphicsContext::GraphicsContext(GLFWwindow* handle)
		: m_Handle(handle)
	{
	}

	void GraphicsContext::Init()
	{
		glfwMakeContextCurrent(m_Handle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		FORGE_ASSERT(status != 0, "Failed to initialize Glad");

		FORGE_INFO("OpenGL Info:");
		FORGE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		FORGE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		FORGE_INFO("  Version: {0}", glGetString(GL_VERSION));

		FORGE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Forge requires at least OpenGL version 4.5!");

		GraphicsCache::Init();
	}

	void GraphicsContext::SwapBuffers()
	{
		glfwSwapBuffers(m_Handle);
	}

}

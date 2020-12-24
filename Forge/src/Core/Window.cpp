#include "ForgePch.h"
#include "Window.h"

namespace Forge
{

	namespace Detail
	{
		void OnGlError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
		{
			if (type == GL_DEBUG_TYPE_ERROR)
				FORGE_FATAL(message);
		}
	}

	static bool s_GlfwInitialized = false;

	Window::Window(const WindowProps& props)
		: m_Data(), m_Handle(nullptr), m_ShouldClose(false), Events()
	{
		InitFromProps(props);
	}

	void Window::Update()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
		if (!m_ShouldClose && glfwWindowShouldClose(m_Handle.get()) == GLFW_TRUE)
		{
			m_ShouldClose = true;
			Events.Close.Trigger();
		}
	}

	void Window::InitFromProps(const WindowProps& props)
	{
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.Title = props.Title;

		if (!s_GlfwInitialized)
		{
			int result = glfwInit();
			FORGE_ASSERT(result != 0, "Failed to initialize GLFW");
			s_GlfwInitialized = true;
		}

#ifdef FORGE_DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
		m_Handle = Handle(glfwCreateWindow(int(m_Data.Width), int(m_Data.Height), m_Data.Title.c_str(), nullptr, nullptr));

		m_Context = std::make_unique<GraphicsContext>(m_Handle.get());
		m_Context->Init();

		glfwSetWindowUserPointer(m_Handle.get(), &m_Data);
		glfwSwapInterval(1);

		glDebugMessageCallback(Detail::OnGlError, nullptr);
	}

}

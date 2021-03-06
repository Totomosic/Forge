#include "ForgePch.h"
#include "Window.h"

namespace Forge
{

	static bool s_GlfwInitialized = false;

	Window::Window(const WindowProps& props)
		: m_Data(), m_Handle(nullptr), m_ShouldClose(false), Events(m_Data.Events)
	{
		InitFromProps(props);
	}

	void Window::EnableVSync()
	{
		glfwSwapInterval(1);
	}

	void Window::DisableVSync()
	{
		glfwSwapInterval(0);
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
		m_Data.Title = props.Title;
		m_Data.Framebuffer = Framebuffer::CreateWindowFramebuffer(props.Width, props.Height);

		if (!s_GlfwInitialized)
		{
			int result = glfwInit();
			FORGE_ASSERT(result != 0, "Failed to initialize GLFW");
			s_GlfwInitialized = true;
		}

#ifdef FORGE_DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
		m_Handle = Handle(glfwCreateWindow(int(props.Width), int(props.Height), m_Data.Title.c_str(), nullptr, nullptr));

		m_Context = std::make_unique<GraphicsContext>(m_Handle.get());
		m_Context->Init();

		glfwSetWindowUserPointer(m_Handle.get(), &m_Data);
		glfwSwapInterval(1);

		glfwSetWindowSizeCallback(m_Handle.get(), [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowResize evt;
			evt.OldWidth = data.Framebuffer->GetWidth();
			evt.OldHeight = data.Framebuffer->GetHeight();
			evt.NewWidth = width;
			evt.NewHeight = height;

			data.Framebuffer->SetSize(uint32_t(width), uint32_t(height));

			data.Events.Resize.Trigger(evt);
		});
	}

}

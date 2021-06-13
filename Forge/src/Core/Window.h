#pragma once
#include "ForgePch.h"
#include "EventEmitter.h"
#include "GraphicsContext.h"
#include "WindowEvents.h"
#include "Viewport.h"
#include "Renderer/Framebuffer.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Forge
{

	namespace Detail
	{
	
		struct FORGE_API WindowDestructor
		{
		public:
			void operator()(GLFWwindow* handle) const
			{
				glfwDestroyWindow(handle);
			}
		};

	}

	struct FORGE_API WindowProps
	{
	public:
		uint32_t Width = 1280;
		uint32_t Height = 720;
		std::string Title = "Forge";
	};

	class FORGE_API Window
	{
	private:
		struct FORGE_API WindowEvents
		{
		public:
			EventEmitter<WindowResize> Resize;
			EventEmitter<WindowClose> Close;
		};

		struct FORGE_API WindowData
		{
		public:
			std::string Title;
			WindowEvents Events;
			Ref<Forge::Framebuffer> Framebuffer;
		};

		using Handle = std::unique_ptr<GLFWwindow, Detail::WindowDestructor>;

		Handle m_Handle;
		std::unique_ptr<GraphicsContext> m_Context;
		WindowData m_Data;
		bool m_ShouldClose;

	public:
		WindowEvents& Events;

	public:
		Window(const WindowProps& props);

		inline const GraphicsContext& GetContext() const { return *m_Context; }
		inline GraphicsContext& GetContext() { return *m_Context; }
		inline const Ref<Framebuffer>& GetFramebuffer() const { return m_Data.Framebuffer; }

		inline bool ShouldClose() const { return m_ShouldClose; }
		inline uint32_t GetWidth() const { return GetFramebuffer()->GetWidth(); }
		inline uint32_t GetHeight() const { return GetFramebuffer()->GetHeight(); }
		inline void* GetNativeHandle() const { return (void*)m_Handle.get(); }
		inline float GetAspectRatio() const { return GetFramebuffer()->GetAspect(); }
		inline void Close() { m_ShouldClose = true; }

		void Update();

	private:
		void InitFromProps(const WindowProps& props);

	};

}

#pragma once
#include "ForgePch.h"
#include "EventEmitter.h"
#include "GraphicsContext.h"
#include "WindowEvents.h"

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
			uint32_t Width;
			uint32_t Height;
			WindowEvents Events;
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

		inline uint32_t GetWidth() const { return m_Data.Width; }
		inline uint32_t GetHeight() const { return m_Data.Height; }
		inline void* GetNativeHandle() const { return (void*)m_Handle.get(); }

		void Update();

	private:
		void InitFromProps(const WindowProps& props);

	};

}

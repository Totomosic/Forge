#pragma once
#include "Window.h"
#include "Scene/Scene.h"
#include "Renderer/Renderer3D.h"
#include "Layer.h"
#include "ImGuiLayer.h"

#include <chrono>

namespace Forge
{

	class FORGE_API Application
	{
	private:
		Window m_Window;
		std::vector<Scope<Scene>> m_Scenes;
		Renderer3D m_Renderer;

		std::chrono::time_point<std::chrono::high_resolution_clock> m_PrevFrameTime;

		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;

	public:
		Application(const WindowProps& props = {});

		inline bool ShouldExit() const { return m_Window.ShouldClose(); }
		inline Window& GetWindow() { return m_Window; }
		inline Renderer3D& GetRenderer() { return m_Renderer; }
		inline Timestep GetTimestep() const { return float(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_PrevFrameTime).count()) / 1e6; }

		void EnableImGui();

		void SetClearColor(const Color& color);
		Scene& CreateScene();

		template<typename T, typename... Args>
		T& PushLayer(Args&&... args)
		{
			T& layer = (T&)m_LayerStack.PushLayer(std::make_unique<T>(std::forward<Args>(args)...));
			layer.OnAttach(*this);
			return layer;
		}

		template<typename T, typename... Args>
		T& PushOverlay(Args&&... args)
		{
			T& layer = (T&)m_LayerStack.PushOverlay(std::make_unique<T>(std::forward<Args>(args)...));
			layer.OnAttach(*this);
			return layer;
		}

		void OnUpdate();

	};

}

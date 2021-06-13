#pragma once
#include "Timestep.h"

namespace Forge
{

	class Application;

	class FORGE_API Layer
	{
	public:
		virtual ~Layer() = default;

		virtual void OnAttach(Application& app) {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImGuiRender() {}
	};

	class FORGE_API LayerStack
	{
	private:
		std::vector<std::unique_ptr<Layer>> m_Layers;
		uint32_t m_InsertIndex = 0;

	public:
		LayerStack() = default;
		~LayerStack();
		
		Layer& PushLayer(std::unique_ptr<Layer>&& layer);
		Layer& PushOverlay(std::unique_ptr<Layer>&& layer);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* layer);

		inline std::vector<std::unique_ptr<Layer>>::iterator begin() { return m_Layers.begin(); }
		inline std::vector<std::unique_ptr<Layer>>::iterator end() { return m_Layers.end(); }
		inline std::vector<std::unique_ptr<Layer>>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		inline std::vector<std::unique_ptr<Layer>>::reverse_iterator rend() { return m_Layers.rend(); }

		inline std::vector<std::unique_ptr<Layer>>::const_iterator begin() const { return m_Layers.begin(); }
		inline std::vector<std::unique_ptr<Layer>>::const_iterator end()	const { return m_Layers.end(); }
		inline std::vector<std::unique_ptr<Layer>>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
		inline std::vector<std::unique_ptr<Layer>>::const_reverse_iterator rend() const { return m_Layers.rend(); }
	};

}

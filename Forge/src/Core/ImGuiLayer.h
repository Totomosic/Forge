#pragma once
#include "Layer.h"

namespace Forge
{

	class Application;

	class FORGE_API ImGuiLayer : public Layer
	{
	private:
		Application* m_Application;

	public:
		ImGuiLayer();
		
		virtual void OnAttach(Forge::Application& app) override;
		virtual void OnDetach() override;
		
		void Begin();
		void End();
	};

}

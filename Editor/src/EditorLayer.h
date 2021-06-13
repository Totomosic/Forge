#pragma once
#include "Forge.h"

namespace Editor
{

	constexpr uint8_t UI_LAYER = 32;

	class EditorLayer : public Forge::Layer
	{
	public:
		EditorLayer() = default;

		virtual void OnAttach(Forge::Application& app) override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Forge::Timestep ts) override;
		virtual void OnImGuiRender() override;
	};

}

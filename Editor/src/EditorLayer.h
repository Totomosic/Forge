#pragma once
#include "Forge.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/AssetBrowserPanel.h"

#include <imgui.h>
#include <ImGuizmo.h>

namespace Editor
{

	constexpr uint8_t UI_LAYER = 32;

	class EditorLayer : public Forge::Layer
	{
	private:
		Forge::Application* m_Application;
		Forge::Ref<Forge::RenderTexture> m_SceneTexture;

		SceneHierarchyPanel m_SceneHierarchy;
		AssetBrowserPanel m_AssetBrowser;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportSize;

		Forge::Entity m_Camera;
		ImGuizmo::OPERATION m_GuizmoOperation = ImGuizmo::OPERATION::TRANSLATE;

	public:
		EditorLayer() = default;

		virtual void OnAttach(Forge::Application& app) override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Forge::Timestep ts) override;
		virtual void OnImGuiRender() override;
	};

}

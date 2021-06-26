#pragma once
#include "Forge.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/AssetBrowserPanel.h"
#include "Panels/LibraryBrowserPanel.h"
#include "Panels/PostProcessingPanel.h"

#include <imgui.h>
#include <ImGuizmo.h>

namespace Editor
{

	constexpr uint8_t UI_LAYER = 32;

	class EditorLayer : public Forge::Layer
	{
	private:
		Forge::Application* m_Application;
		Forge::Scene* m_Scene;
		Forge::Ref<Forge::RenderTexture> m_SceneTexture;

		SceneHierarchyPanel m_SceneHierarchy;
		AssetBrowserPanel m_AssetBrowser;
		LibraryBrowserPanel m_LibraryBrowser;
		PostProcessingPanel m_PostProcessing;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportBounds[2];
		glm::vec2 m_ViewportSize;

		Forge::Entity m_Camera;
		bool m_OperationLocked = false;
		ImGuizmo::OPERATION m_GuizmoOperation = ImGuizmo::OPERATION::TRANSLATE;

		Forge::Timestep m_Timestep;

	public:
		EditorLayer() = default;

		virtual void OnAttach(Forge::Application& app) override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Forge::Timestep ts) override;
		virtual void OnImGuiRender() override;

	private:
		void NewScene();
	};

}

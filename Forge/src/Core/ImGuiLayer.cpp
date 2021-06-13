#include "ForgePch.h"
#include "ImGuiLayer.h"
#include "Application.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <ImGuizmo.h>

namespace Forge
{

	ImGuiLayer::ImGuiLayer() : Layer(),
		m_Application(nullptr), m_BlockEvents(true)
	{
	}

	void ImGuiLayer::OnAttach(Application& app)
	{
		m_Application = &app;
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();
		GLFWwindow* window = (GLFWwindow*)m_Application->GetWindow().GetNativeHandle();

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 430");

		io.DisplaySize = ImVec2((float)m_Application->GetWindow().GetWidth(), (float)m_Application->GetWindow().GetHeight());
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)m_Application->GetWindow().GetWidth(), (float)m_Application->GetWindow().GetHeight());
	}

}

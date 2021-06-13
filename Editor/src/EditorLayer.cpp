#include "EditorLayer.h"
using namespace Forge;

namespace Editor
{

	void EditorLayer::OnAttach(Forge::Application& app)
	{
		m_Application = &app;
		float width = app.GetWindow().GetWidth();
		float height = app.GetWindow().GetHeight();

		m_SceneTexture = RenderTexture::Create(1920, 1080);

		Scene& scene = app.CreateScene();
		m_Camera = scene.CreateCamera(Frustum::Perspective(PI / 3.0f, app.GetWindow().GetAspectRatio(), 0.1f, 1000.0f));
		m_Camera.GetComponent<CameraComponent>().RenderTarget = *m_SceneTexture;
		m_Camera.GetComponent<CameraComponent>().LayerMask &= ~FORGE_LAYERS(UI_LAYER);
		m_Camera.GetComponent<CameraComponent>().Viewport = m_SceneTexture->GetFramebuffer()->GetViewport();

		m_Camera.GetTransform().SetPosition({ 0, 5, 5 });
		m_Camera.GetTransform().Rotate(-PI / 4.0f, { 1, 0, 0 });

		Entity groundPlane = scene.CreateEntity("Ground");
		groundPlane.AddComponent<ModelRendererComponent>(
			Model::Create(
				GraphicsCache::GridMesh(2, 2),
				GraphicsCache::LitColorMaterial(COLOR_WHITE)
			)
		);
		groundPlane.GetTransform().SetPosition({ 0, 0, 0 });
		groundPlane.GetTransform().SetScale({ 15, 1, 15 });

		Entity sun = scene.CreateEntity("Sun");
		sun.GetTransform().SetPosition({ 0, 5, 0 });
		LightSourceComponent& lightSource = sun.AddComponent<LightSourceComponent>();

		m_SceneHierarchy.SetScene(&scene);
		m_ViewportSize = { width, height };

		Input::OnKeyPressed.AddEventListener([&](const KeyCode& key)
		{
			if (key == KeyCode::Escape)
				m_SceneHierarchy.SetSelectedEntity({});
			return false;
		});
	}

	void EditorLayer::OnDetach()
	{
	
	}

	void EditorLayer::OnUpdate(Forge::Timestep ts)
	{
		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f)
		{
			m_Camera.GetComponent<CameraComponent>().Frustum = Frustum::Perspective(PI / 3.0f, m_ViewportSize.x / m_ViewportSize.y, 0.01f, 1000.0f);
		}

		if (m_ViewportFocused)
		{
			float speed = 5.0f;
			TransformComponent& transform = m_Camera.GetTransform();
			if (Input::IsKeyDown(KeyCode::A))
			{
				transform.Translate(transform.GetRight() * -speed * ts.Seconds());
			}
			if (Input::IsKeyDown(KeyCode::D))
			{
				transform.Translate(transform.GetRight() * speed * ts.Seconds());
			}
			if (Input::IsKeyDown(KeyCode::W))
			{
				transform.Translate(transform.GetForward() * speed * ts.Seconds());
			}
			if (Input::IsKeyDown(KeyCode::S))
			{
				transform.Translate(transform.GetForward() * -speed * ts.Seconds());
			}
			if (Input::IsMouseButtonDown(MouseButton::Left) && !ImGuizmo::IsOver())
			{
				float horizontalScale = 0.003f;
				float verticalScale = 0.003f;
				glm::vec2 delta = Input::GetRelMousePosition();
				transform.Rotate(-delta.x * horizontalScale, glm::vec3{ 0, 1, 0 }, Space::World);
				transform.Rotate(delta.y * verticalScale, glm::vec3{ 1, 0, 0 }, Space::Local);
			}
		}
	}

	void EditorLayer::OnImGuiRender()
	{
		bool fullscreen = true;
		bool dockspaceOpen = true;

		ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Editor", &dockspaceOpen, windowFlags);
		ImGui::PopStyleVar();

		if (fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiID dockspaceId = ImGui::GetID("Dockspace");
		ImGui::DockSpace(dockspaceId, ImVec2(0, 0), dockspaceFlags);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);1
				if (ImGui::MenuItem("New", "Ctrl+N"))
				{
				}

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
				{
				}

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
				{
				}

				if (ImGui::MenuItem("Exit"))
				{
					m_Application->GetWindow().Close();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Tools"))
			{
				if (ImGui::MenuItem("Translate", "W"))
				{
					m_GuizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
				}

				if (ImGui::MenuItem("Rotate", "E"))
				{
					m_GuizmoOperation = ImGuizmo::OPERATION::ROTATE;
				}

				if (ImGui::MenuItem("Scale", "R"))
				{
					m_GuizmoOperation = ImGuizmo::OPERATION::SCALE;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		m_SceneHierarchy.OnImGuiRender();

		ImGui::Begin("Scene");

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		ImVec2 viewportMin = ImGui::GetWindowContentRegionMin();
		ImVec2 viewportMax = ImGui::GetWindowContentRegionMax();
		ImVec2 viewportOffset = ImGui::GetWindowPos();
		glm::vec2 viewportBounds[2];
		viewportBounds[0] = { viewportMin.x + viewportOffset.x, viewportMin.y + viewportOffset.y };
		viewportBounds[1] = { viewportMax.x + viewportOffset.x, viewportMax.y + viewportOffset.y };

		ImVec2 sceneSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { sceneSize.x, sceneSize.y };
		uint32_t textureId = m_SceneTexture->GetId();
		ImGui::Image((void*)textureId, ImVec2{ sceneSize.x, sceneSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (m_SceneHierarchy.GetSelectedEntity())
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(viewportBounds[0].x, viewportBounds[0].y, viewportBounds[1].x - viewportBounds[0].x, viewportBounds[1].y - viewportBounds[0].y);

			const glm::mat4& projection = m_Camera.GetComponent<CameraComponent>().Frustum.ProjectionMatrix;
			glm::mat4 viewMatrix = m_Camera.GetTransform().GetInverseMatrix();

			TransformComponent& transform = m_SceneHierarchy.GetSelectedEntity().GetTransform();
			glm::mat4 transformMatrix = transform.GetMatrix();

			ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projection), m_GuizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(transformMatrix), nullptr, nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 position;
				glm::quat rotation;
				glm::vec3 scale;
				Math::DecomposeTransform(transformMatrix, position, rotation, scale);

				transform.SetPosition(position);
				transform.SetScale(scale);
				transform.SetRotation(rotation);
			}
		}

		ImGui::End();

		ImGui::End();
	}

}

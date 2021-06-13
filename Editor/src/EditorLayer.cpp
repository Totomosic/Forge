#include "EditorLayer.h"
using namespace Forge;

#include <imgui.h>

namespace Editor
{

	void EditorLayer::OnAttach(Forge::Application& app)
	{
		float width = app.GetWindow().GetWidth();
		float height = app.GetWindow().GetHeight();

		float viewportWidthProp = 0.8f;
		float viewportHeightProp = 0.8f;

		Ref<RenderTexture> viewportRenderTarget = RenderTexture::Create(width * viewportWidthProp, height * viewportHeightProp);

		Scene& scene = app.CreateScene();
		Entity camera = scene.CreateCamera(Frustum::Perspective(PI / 3.0f, app.GetWindow().GetAspectRatio(), 0.1f, 1000.0f));
		camera.GetComponent<CameraComponent>().RenderTarget = *viewportRenderTarget;
		camera.GetComponent<CameraComponent>().LayerMask &= ~FORGE_LAYERS(UI_LAYER);
		camera.GetComponent<CameraComponent>().Viewport = viewportRenderTarget->GetFramebuffer()->GetViewport();

		camera.GetTransform().SetPosition({ 0, 5, 5 });
		camera.GetTransform().Rotate(-PI / 4.0f, { 1, 0, 0 });

		Entity groundPlane = scene.CreateEntity();
		groundPlane.AddComponent<ModelRendererComponent>(
			Model::Create(
				GraphicsCache::GridMesh(2, 2),
				GraphicsCache::LitColorMaterial(COLOR_WHITE)
			)
			);
		groundPlane.GetTransform().SetPosition({ 0, 0, 0 });
		groundPlane.GetTransform().SetScale({ 15, 1, 15 });

		Entity sun = scene.CreateEntity();
		sun.GetTransform().SetPosition({ 0, 5, 0 });
		LightSourceComponent& lightSource = sun.AddComponent<LightSourceComponent>();
		lightSource.Color = { 100, 100, 255 };

		Entity uiCamera = scene.CreateCamera(Frustum::Orthographic(0, width, 0, height));
		uiCamera.GetComponent<CameraComponent>().ClearColor = { 25, 25, 25 };
		uiCamera.GetComponent<CameraComponent>().LayerMask = FORGE_LAYERS(UI_LAYER);
		Entity viewport = scene.CreateEntity(UI_LAYER);
		viewport.AddComponent<ModelRendererComponent>(
			GraphicsCache::SquareModel(
				width * viewportWidthProp,
				height * viewportHeightProp,
				GraphicsCache::DefaultTextureMaterial(viewportRenderTarget)
			)
			);
		viewport.GetTransform().SetPosition({ width * (1.0f - viewportWidthProp / 2.0f), height * (1.0f - viewportHeightProp / 2.0f), 0.0f });
	}

	void EditorLayer::OnDetach()
	{
	
	}

	void EditorLayer::OnUpdate(Forge::Timestep ts)
	{
	
	}

	void EditorLayer::OnImGuiRender()
	{
		ImGui::Begin("Test");
		ImGui::Text("Entity");
		ImGui::End();
	}

}

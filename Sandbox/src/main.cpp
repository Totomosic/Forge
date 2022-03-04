#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Forge.h"

using namespace Forge;

int main()
{
	ForgeInstance::Init();

	WindowProps props;
	Application app(props);

	Scene& scene = app.CreateScene();
	Entity camera = scene.CreateCamera(Frustum::Perspective(PI / 3.0f, app.GetWindow().GetAspectRatio(), 0.1f, 1000.0f));
	camera.GetComponent<TransformComponent>().SetLocalPosition({ 0, 0, 10 });
	camera.GetComponent<CameraComponent>().Viewport = app.GetWindow().GetFramebuffer()->GetViewport();
	camera.GetComponent<CameraComponent>().ClearColor = COLOR_BLACK;

	Entity cube = scene.CreateEntity();
	cube.AddComponent<ModelRendererComponent>(
		Model::Create(
			GraphicsCache::SphereMesh(),
			GraphicsCache::PbrColorMaterial(COLOR_WHITE)
		)
	);

	Entity ground = scene.CreateEntity();
	ground.AddComponent<ModelRendererComponent>(
		Model::Create(
			GraphicsCache::GridMesh(2, 2),
			GraphicsCache::PbrColorMaterial(COLOR_GREEN)
		)
	);
	ground.GetTransform().SetLocalScale({ 10, 1, 10 });
	ground.GetTransform().SetLocalPosition({ 0, -0.5f, 0 });

	Entity sun = scene.CreateEntity();
	DirectionalLightComponent& light = sun.AddComponent<DirectionalLightComponent>();
	light.CreateShadowPass(4096, 4096);
	TransformComponent& sunTransform = sun.GetTransform();
	sunTransform.Rotate(-PI / 4.0f, glm::vec3{ 1, 0, 0 });
	sunTransform.SetLocalPosition({ 0, 10, 10 });

	app.GetWindow().Events.Resize.AddEventListener([&camera, &app](const WindowResize& evt)
	{
		CameraComponent& c = camera.GetComponent<CameraComponent>();
		c.Frustum = Frustum::Perspective(PI / 3.0f, app.GetWindow().GetAspectRatio(), 0.1f, 1000.0f);
		c.Viewport = app.GetWindow().GetFramebuffer()->GetViewport();
		return true;
	});

	while (!app.ShouldExit())
	{
		Timestep ts = app.GetTimestep();

		TransformComponent& transform = camera.GetTransform();
		float speed = 10 * ts.Seconds();
		if (Input::IsKeyDown(KeyCode::W))
			transform.Translate(transform.GetForward() * speed);
		if (Input::IsKeyDown(KeyCode::S))
			transform.Translate(transform.GetForward() * -speed);
		if (Input::IsKeyDown(KeyCode::D))
			transform.Translate(transform.GetRight() * speed);
		if (Input::IsKeyDown(KeyCode::A))
			transform.Translate(transform.GetRight() * -speed);

		if (Input::IsMouseButtonDown(MouseButton::Left))
		{
			float sensitivity = 0.003f;
			glm::vec2 delta = Input::GetRelMousePosition();
			transform.Rotate(-delta.x * sensitivity, glm::vec3{ 0, 1, 0 }, Space::World);
			transform.Rotate(delta.y * sensitivity, glm::vec3{ 1, 0, 0 }, Space::Local);
		}

		app.OnUpdate();
	}

	return 0;
}

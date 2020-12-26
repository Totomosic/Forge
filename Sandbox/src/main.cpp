#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Forge.h"

using namespace Forge;

int main()
{
	ForgeInstance::Init();

	bool running = true;

	WindowProps props;
	Window window(props);

	Input::SetWindow(&window);

	window.Events.Close.AddEventListener([&](const WindowClose& evt)
	{
		running = false;
		return true;
	});

	window.Events.Resize.AddEventListener([](const WindowResize& evt)
	{
		RenderCommand::SetViewport(0, 0, evt.NewWidth, evt.NewHeight);
		return false;
	});
	
	RenderCommand::Init();
	Renderer3D renderer;

	GltfReader reader("scene.gltf");

	Scene scene;
	Entity camera = scene.CreateEntity();
	camera.AddComponent<CameraComponent>(glm::perspective(PI / 3.0f, window.GetAspectRatio(), 0.1f, 1000.0f));
	camera.GetComponent<TransformComponent>().SetPosition({ 0, 0, 10 });

	Ref<Texture2D> texture = Texture2D::Create("textures/material_0_diffuse.png");

	for (const Ref<Mesh>& mesh : reader.GetMeshes())
	{
		Ref<Model> model = Model::Create(mesh, GraphicsCache::LitTextureMaterial(texture));
		Entity entity = scene.CreateEntity();
		entity.AddComponent<ModelRendererComponent>(model);
	}

	Entity sun = scene.CreateEntity();
	sun.GetTransform().SetPosition({ 0, 100, 0 });
	sun.AddComponent<LightSourceComponent>();

	RenderCommand::SetClearColor(COLOR_BLACK);

	while (running)
	{
		TransformComponent& transform = camera.GetTransform();
		float speed = 0.1f;
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

		RenderCommand::Clear();

		scene.OnUpdate({}, renderer);
		renderer.Flush();

		window.Update();
	}

	return 0;
}

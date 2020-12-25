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

	Input::OnMouseMoved.AddEventListener([&](const MouseMove& evt)
	{
		std::cout << Input::GetMouseX() << ", " << Input::GetMouseY() << std::endl;
		return false;
	});

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

	Scene scene;
	Entity camera = scene.CreateEntity();
	camera.AddComponent<CameraComponent>(glm::ortho(0.0f, float(window.GetWidth()), 0.0f, float(window.GetHeight())));
	// camera.GetComponent<TransformComponent>().SetPosition({ 0, 0, 10 });

	Ref<Model> model = Model::Create(GraphicsCache::SquareMesh(), GraphicsCache::DefaultColorMaterial(COLOR_BLACK));
	Entity square = scene.CreateEntity();
	square.AddComponent<ModelRendererComponent>(model);
	square.GetComponent<TransformComponent>().SetScale({ 100, 100, 1 });

	RenderCommand::SetClearColor(COLOR_RED);

	while (running)
	{
		RenderCommand::Clear();
	
		square.GetComponent<TransformComponent>().SetPosition({ Input::GetMousePosition(), 0 });

		scene.OnUpdate({}, renderer);
		renderer.Flush();

		window.Update();
	}

	return 0;
}

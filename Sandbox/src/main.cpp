#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

	Ref<Model> model = Model::Create(GraphicsCache::SquareMesh(), GraphicsCache::DefaultColorMaterial(COLOR_BLACK));

	RenderCommand::SetClearColor(COLOR_RED);

	CameraData camera;
	camera.ViewMatrix = glm::mat4(1.0f);
	camera.ProjectionMatrix = glm::ortho(0.0f, float(window.GetWidth()), 0.0f, float(window.GetHeight()));

	while (running)
	{
		RenderCommand::Clear();
		
		renderer.BeginScene(camera);
		renderer.RenderModel(model, glm::translate(glm::mat4(1.0f), glm::vec3{ Input::GetMousePosition(), 0.0f }) * glm::scale(glm::mat4(1.0f), glm::vec3{ 100, 100, 1 }));
		renderer.RenderModel(model, glm::translate(glm::mat4(1.0f), glm::vec3{ Input::GetMousePosition() + glm::vec2{ 150, 75 }, 0.0f }) * glm::scale(glm::mat4(1.0f), glm::vec3{ 200, 100, 1 }));
		renderer.EndScene();
		renderer.Flush();

		window.Update();
	}

	return 0;
}

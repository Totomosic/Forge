#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Forge.h"
using namespace Forge;

#include "Terrain.h"

int main()
{
	ForgeInstance::Init();

	WindowProps props;
	Application app(props);

	Ref<Shader> terrainShader = Shader::CreateFromFile("res/Terrain.shader");

	Scene& scene = app.CreateScene();
	Entity camera = scene.CreateEntity();
	camera.AddComponent<CameraComponent>(glm::perspective(PI / 3.0f, app.GetWindow().GetAspectRatio(), 0.1f, 1000.0f));
	camera.GetComponent<TransformComponent>().SetPosition({ 0, 12, 10 });

	Entity sun = scene.CreateEntity();
	sun.GetTransform().SetPosition({ 0, 100, 0 });
	sun.AddComponent<LightSourceComponent>();
	sun.GetComponent<LightSourceComponent>().Ambient = 0.5f;

	Terrain terrain({ -100, -1450, 300 }, -10.0f);
	Ref<Mesh> mesh = terrain.GenerateMesh({ 10, 20, 10 }, { 100, 200, 100 }, 2.0f);
	Ref<Material> material = CreateRef<Material>(terrainShader);
	material->GetUniforms().AddUniform("u_Color", Color{ 112, 72, 60 });
	Ref<Model> model = Model::Create(mesh, material);

	Entity water = scene.CreateEntity();
	Ref<Mesh> waterMesh = GraphicsCache::SquareMesh();
	Ref<Material> waterMaterial = GraphicsCache::LitColorMaterial(COLOR_BLUE.WithAlpha(150));
	water.AddComponent<ModelRendererComponent>(Model::Create(waterMesh, waterMaterial));
	water.GetTransform().SetScale({ 10.0f, 10.0f, 1.0f });
	water.GetTransform().Rotate(-PI / 2.0f, glm::vec3{ 1, 0, 0 });
	water.GetTransform().SetPosition({ 0, 10, 0 });

	Entity terrainEntity = scene.CreateEntity();
	terrainEntity.AddComponent<ModelRendererComponent>(model);

	// camera.GetComponent<CameraComponent>().ClippingPlanes.push_back(glm::vec4{ 0.0f, 1.0f, 0.0f, -10.0f });

	while (!app.ShouldExit())
	{
		Timestep ts = app.GetTimestep();

		TransformComponent& transform = camera.GetTransform();
		float speed = 5 * ts.Seconds();
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
			float sensitivity = 0.002f;
			glm::vec2 delta = Input::GetRelMousePosition();
			transform.Rotate(-delta.x * sensitivity, glm::vec3{ 0, 1, 0 }, Space::World);
			transform.Rotate(delta.y * sensitivity, glm::vec3{ 1, 0, 0 }, Space::Local);
		}

		app.OnUpdate();
	}

	return 0;
}

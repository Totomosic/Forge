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
	Ref<Shader> waterShader = Shader::CreateFromFile("res/Water.shader");
	Ref<Shader> skyboxShader = Shader::CreateFromFile("res/Skybox.shader");
	Ref<Material> waterMaterial = CreateRef<Material>(waterShader);

	Ref<TextureCube> skyboxTexture = TextureCube::Create(
		"res/skybox2/front.jpg",
		"res/skybox2/back.jpg",
		"res/skybox2/left.jpg",
		"res/skybox2/right.jpg",
		"res/skybox2/bottom.jpg",
		"res/skybox2/top.jpg"
	);

	Ref<Material> skyboxMaterial = CreateRef<Material>(skyboxShader);
	skyboxMaterial->GetUniforms().AddUniform("u_Texture", skyboxTexture);

	Scene& scene = app.CreateScene();
	Entity camera = scene.CreateCamera(glm::perspective(PI / 3.0f, app.GetWindow().GetAspectRatio(), 0.1f, 1000.0f));
	camera.GetComponent<TransformComponent>().SetPosition({ 0, 12, 10 });
	camera.GetComponent<CameraComponent>().ClearColor = SKY_BLUE;

	app.GetWindow().Events.Resize.AddEventListener([camera](const WindowResize& evt) mutable
	{
		CameraComponent& cc = camera.GetComponent<CameraComponent>();
		cc.Viewport.Width = evt.NewWidth;
		cc.Viewport.Height = evt.NewHeight;
		return false;
	});

	Entity sun = scene.CreateEntity();
	sun.GetTransform().SetPosition({ 0, 25, 0 });
	sun.AddComponent<LightSourceComponent>();
	sun.GetComponent<LightSourceComponent>().Ambient = 0.5f;
	scene.AddToAllLayers(sun);
	
	Terrain terrain({ -100, -1450, 300 }, -10.0f);
	Ref<Mesh> mesh = terrain.GenerateMesh({ 10, 20, 10 }, { 100, 200, 100 }, 2.0f);
	Ref<Material> material = CreateRef<Material>(terrainShader);
	material->GetUniforms().AddUniform("u_Color", Color{ 112, 72, 60 });
	Ref<Model> model = Model::Create(mesh, material);

	Entity water = scene.CreateEntity();
	Ref<Mesh> waterMesh = GraphicsCache::SquareMesh();
	water.AddComponent<ModelRendererComponent>(Model::Create(waterMesh, waterMaterial));
	water.GetTransform().SetScale({ 10.0f, 10.0f, 1.0f });
	water.GetTransform().Rotate(-PI / 2.0f, glm::vec3{ 1, 0, 0 });
	water.GetTransform().SetPosition({ 0, 10, 0 });
	scene.SetLayer(water, 1);

	Entity terrainEntity = scene.CreateEntity();
	terrainEntity.AddComponent<ModelRendererComponent>(model);

	Entity skybox = scene.CreateEntity();
	skybox.AddComponent<ModelRendererComponent>(Model::Create(GraphicsCache::CubeMesh(), skyboxMaterial));
	skybox.GetTransform().SetPosition({ 0, 0, 0 });
	skybox.GetTransform().SetScale({ 200, 200, 200 });

	Ref<RenderTexture> refractionTexture = RenderTexture::Create(1280, 720);
	Entity refractionCamera = scene.CreateCamera(glm::perspective(PI / 3.0f, app.GetWindow().GetAspectRatio(), 0.1f, 1000.0f));
	refractionCamera.GetComponent<CameraComponent>().ClearColor = SKY_BLUE;
	refractionCamera.GetComponent<CameraComponent>().RenderTarget = refractionTexture->GetFramebuffer();
	refractionCamera.GetComponent<CameraComponent>().LayerMask = 1;
	refractionCamera.GetComponent<CameraComponent>().ClippingPlanes.push_back(glm::vec4{ 0.0f, -1.0f, 0.0f, 10.0f });

	Ref<RenderTexture> reflectionTexture = RenderTexture::Create(1280, 720);
	Entity reflectionCamera = scene.CreateCamera(glm::perspective(PI / 3.0f, app.GetWindow().GetAspectRatio(), 0.1f, 1000.0f));
	reflectionCamera.GetComponent<CameraComponent>().ClearColor = SKY_BLUE;
	reflectionCamera.GetComponent<CameraComponent>().RenderTarget = reflectionTexture->GetFramebuffer();
	reflectionCamera.GetComponent<CameraComponent>().LayerMask = 1;
	reflectionCamera.GetComponent<CameraComponent>().ClippingPlanes.push_back(glm::vec4{ 0.0f, 1.0f, 0.0f, -10.0f });

	Ref<Texture2D> normal = Texture2D::Create("res/normal.png");
	Ref<Texture2D> dudv = Texture2D::Create("res/waterDUDV.png");
	// normal->GenerateMipmaps();
	dudv->GenerateMipmaps();

	waterMaterial->GetUniforms().AddUniform("u_RefractionTexture", refractionTexture);
	waterMaterial->GetUniforms().AddUniform("u_ReflectionTexture", reflectionTexture);
	waterMaterial->GetUniforms().AddUniform("u_DepthTexture", refractionTexture->GetFramebuffer()->GetTexture(ColorBuffer::Depth));
	waterMaterial->GetUniforms().AddUniform("u_CameraPlanes", glm::vec2{ 0.1f, 1000.0f });
	waterMaterial->GetUniforms().AddUniform("u_NormalMap", normal);
	waterMaterial->GetUniforms().AddUniform("u_DUDVMap", dudv);
	waterMaterial->GetUniforms().AddUniform("u_Time", 0.0f);

	float time = 0.0f;

	while (!app.ShouldExit())
	{
		Timestep ts = app.GetTimestep();
		time += ts.Seconds() * 0.08f;

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

		refractionCamera.GetTransform().SetFromTransform(transform);
		reflectionCamera.GetTransform().SetFromTransform(transform);
		reflectionCamera.GetTransform().SetPosition({ transform.GetPosition().x, 20.0f - transform.GetPosition().y, transform.GetPosition().z });
		reflectionCamera.GetTransform().FlipX();

		waterMaterial->GetUniforms().UpdateUniform("u_Time", time);

		app.OnUpdate();
	}

	return 0;
}

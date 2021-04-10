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

	Ref<Material> waterMaterial = Material::CreateFromShaderFile("res/Water.shader");
	Ref<Material> shadowMaterial = Material::CreateFromShaderFile("res/Shadow.shader");

	Ref<TextureCube> skyboxTexture = TextureCube::Create(
		"res/skybox2/front.jpg",
		"res/skybox2/back.jpg",
		"res/skybox2/left.jpg",
		"res/skybox2/right.jpg",
		"res/skybox2/bottom.jpg",
		"res/skybox2/top.jpg"
	);

	Ref<Material> skyboxMaterial = Material::CreateFromShaderFile("res/Skybox.shader");
	skyboxMaterial->GetUniforms().AddUniform("u_Texture", skyboxTexture);

	Ref<RenderTexture> screen = RenderTexture::Create(app.GetWindow().GetWidth() * 2, app.GetWindow().GetHeight() * 2);
	Ref<Material> screenMaterial = Material::CreateFromShaderFile("res/postprocessing/PostProcess.shader");
	screenMaterial->GetUniforms().AddUniform("u_Depth", 1.0f);
	screenMaterial->GetUniforms().AddUniform("u_Texture", screen);

	Scene& scene = app.CreateScene();
	Entity camera = scene.CreateCamera(glm::perspective(PI / 3.0f, app.GetWindow().GetAspectRatio(), 0.1f, 1000.0f));
	camera.GetComponent<TransformComponent>().SetPosition({ 0, 2, 10 });
	camera.GetComponent<CameraComponent>().ClearColor = SKY_BLUE;
	camera.GetComponent<CameraComponent>().CreateShadowPass(2048, 2048);
	camera.GetComponent<CameraComponent>().Shadows.LayerMask = FORGE_LAYER(0);
	camera.GetComponent<CameraComponent>().LayerMask = FORGE_LAYER(0) | FORGE_LAYER(1) | FORGE_LAYER(2);
	camera.GetComponent<CameraComponent>().RenderTarget = screen->GetFramebuffer();
	camera.GetComponent<CameraComponent>().Priority = 1;
	camera.GetComponent<CameraComponent>().Viewport = { 0, 0, screen->GetWidth(), screen->GetHeight() };

	Entity screenCamera = scene.CreateCamera(glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f));
	screenCamera.GetComponent<CameraComponent>().LayerMask = FORGE_LAYER(32);

	Entity screenRectangle = scene.CreateEntity(32);
	screenRectangle.AddComponent<ModelRendererComponent>(Model::Create(GraphicsCache::SquareMesh(), screenMaterial));

	app.GetWindow().Events.Resize.AddEventListener([screenCamera](const WindowResize& evt) mutable
	{
		CameraComponent& cc = screenCamera.GetComponent<CameraComponent>();
		cc.Viewport.Width = evt.NewWidth;
		cc.Viewport.Height = evt.NewHeight;
		return false;
	});

	Entity sun = scene.CreateEntity();
	sun.GetTransform().SetPosition({ 10, 30, 0 });
	sun.AddComponent<LightSourceComponent>();
	sun.GetComponent<LightSourceComponent>().Ambient = 0.3f;
	scene.AddToAllLayers(sun);
	
	Terrain terrain({ -100, -1450, 300 }, -10.0f);
	Ref<Mesh> mesh = terrain.GenerateMesh({ 10, 20, 10 }, { 100, 200, 100 }, 2.0f);
	Ref<Material> material = Material::CreateFromShaderFile("res/Terrain.shader");
	material->GetUniforms().AddUniform("u_Color", Color{ 112, 72, 60 });
	Ref<Model> model = Model::Create(mesh, material);

	Entity water = scene.CreateEntity();
	Ref<Mesh> waterMesh = GraphicsCache::SquareMesh();
	water.AddComponent<ModelRendererComponent>(Model::Create(waterMesh, waterMaterial));
	water.GetTransform().SetScale({ 20.0f, 20.0f, 1.0f });
	water.GetTransform().Rotate(-PI / 2.0f, glm::vec3{ 1, 0, 0 });
	water.GetTransform().SetPosition({ 0, 0, 0 });
	scene.SetLayer(water, 1);

	/*Entity plane = scene.CreateEntity();
	plane.AddComponent<ModelRendererComponent>(Model::Create(GraphicsCache::SquareMesh(), GraphicsCache::LitColorMaterial()));
	plane.GetTransform().SetScale({ 5.0f, 5.0f, 1.0f });
	plane.GetTransform().Rotate(-PI / 2.0f, glm::vec3{ 1, 0, 0 });
	plane.GetTransform().SetPosition({ 0, 1.5f, 0 });*/

	Entity terrainEntity = scene.CreateEntity();
	terrainEntity.AddComponent<ModelRendererComponent>(model);
	terrainEntity.GetTransform().SetPosition({ 0, -10, 0 });

	Entity skybox = scene.CreateEntity();
	skybox.AddComponent<ModelRendererComponent>(Model::Create(GraphicsCache::CubeMesh(), skyboxMaterial));
	skybox.GetTransform().SetPosition({ 0, 0, 0 });
	skybox.GetTransform().SetScale({ 200, 200, 200 });

	Ref<RenderTexture> refractionTexture = RenderTexture::Create(1280, 720);
	Entity refractionCamera = scene.CreateCamera(glm::perspective(PI / 3.0f, app.GetWindow().GetAspectRatio(), 0.1f, 1000.0f));
	refractionCamera.GetComponent<CameraComponent>().ClearColor = SKY_BLUE;
	refractionCamera.GetComponent<CameraComponent>().RenderTarget = refractionTexture->GetFramebuffer();
	refractionCamera.GetComponent<CameraComponent>().LayerMask = FORGE_LAYER(0);
	refractionCamera.GetComponent<CameraComponent>().ClippingPlanes.push_back(glm::vec4{ 0.0f, -1.0f, 0.0f, 0.0f });

	Ref<RenderTexture> reflectionTexture = RenderTexture::Create(1280, 720);
	Entity reflectionCamera = scene.CreateCamera(glm::perspective(PI / 3.0f, app.GetWindow().GetAspectRatio(), 0.1f, 1000.0f));
	reflectionCamera.GetComponent<CameraComponent>().ClearColor = SKY_BLUE;
	reflectionCamera.GetComponent<CameraComponent>().RenderTarget = reflectionTexture->GetFramebuffer();
	reflectionCamera.GetComponent<CameraComponent>().LayerMask = FORGE_LAYER(0);
	reflectionCamera.GetComponent<CameraComponent>().ClippingPlanes.push_back(glm::vec4{ 0.0f, 1.0f, 0.0f, 0.0f });

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

		sun.GetTransform().SetPosition({ 40 * cosf(time / 5.0f), abs(40 * sinf(time / 5.0f)), 0 });

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
		reflectionCamera.GetTransform().SetPosition({ transform.GetPosition().x, 0.0f - transform.GetPosition().y, transform.GetPosition().z });
		reflectionCamera.GetTransform().FlipX();

		waterMaterial->GetUniforms().UpdateUniform("u_Time", time);

		screenMaterial->GetUniforms().UpdateUniform("u_Depth", -transform.GetPosition().y);

		app.OnUpdate();
	}

	return 0;
}

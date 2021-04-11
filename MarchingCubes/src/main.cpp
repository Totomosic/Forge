#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Forge.h"
using namespace Forge;

#include "Terrain.h"

int DEFAULT_LAYER = 0;
int WATER_LAYER = 1;
int SKYBOX_LAYER = 2;

int UI_LAYER = 32;
int TEXTURE_LAYER = 33;

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

	Ref<RenderTexture> screen = RenderTexture::Create(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());
	Ref<Material> screenMaterial = Material::CreateFromShaderFile("res/postprocessing/PostProcess.shader");
	screenMaterial->GetUniforms().AddUniform("u_Depth", 1.0f);
	screenMaterial->GetUniforms().AddUniform("u_Texture", screen);

	Scene& scene = app.CreateScene();
	Entity camera = scene.CreateCamera(Frustum::Perspective(PI / 3.0f, app.GetWindow().GetAspectRatio(), 0.1f, 100.0f));
	camera.GetComponent<TransformComponent>().SetPosition({ 0, 2, 10 });
	camera.GetComponent<CameraComponent>().ClearColor = SKY_BLUE;
	camera.GetComponent<CameraComponent>().CreateShadowPass(2048, 2048);
	camera.GetComponent<CameraComponent>().Shadows.LayerMask = FORGE_LAYERS(DEFAULT_LAYER);
	camera.GetComponent<CameraComponent>().LayerMask = FORGE_LAYERS(DEFAULT_LAYER, WATER_LAYER, SKYBOX_LAYER);
	camera.GetComponent<CameraComponent>().RenderTarget = *screen;
	camera.GetComponent<CameraComponent>().Priority = 1;
	camera.GetComponent<CameraComponent>().Viewport = { 0, 0, screen->GetWidth(), screen->GetHeight() };

	Entity screenCamera = scene.CreateCamera(Frustum::Orthographic(-0.5f, 0.5f, -0.5f, 0.5f));
	screenCamera.GetComponent<CameraComponent>().LayerMask = FORGE_LAYERS(UI_LAYER);

	Entity screenRectangle = scene.CreateEntity(UI_LAYER);
	screenRectangle.AddComponent<ModelRendererComponent>(Model::Create(GraphicsCache::SquareMesh(), screenMaterial));

	app.GetWindow().Events.Resize.AddEventListener([screenCamera](const WindowResize& evt) mutable
	{
		CameraComponent& cc = screenCamera.GetComponent<CameraComponent>();
		cc.Viewport.Width = evt.NewWidth;
		cc.Viewport.Height = evt.NewHeight;
		return false;
	});

	Entity sun = scene.CreateEntity();
	sun.GetTransform().SetPosition({ 30, 10, 0 });
	sun.AddComponent<LightSourceComponent>();
	sun.GetComponent<LightSourceComponent>().Ambient = 0.3f;
	scene.AddToAllLayers(sun);

	Terrain terrain({ -100, -1450, 300 }, -10.0f);
	Ref<Mesh> mesh = terrain.GenerateMesh({ 10, 20, 10 }, { 100, 200, 100 }, 2.0f);
	Ref<Material> material = Material::CreateFromShaderFile("res/Terrain.shader");
	material->GetUniforms().AddUniform("u_Color", Color{ 112, 72, 60 });
	Ref<Model> model = Model::Create(mesh, material);

	Entity water = scene.CreateEntity(WATER_LAYER);
	Ref<Mesh> waterMesh = GraphicsCache::SquareMesh();
	water.AddComponent<ModelRendererComponent>(Model::Create(waterMesh, waterMaterial));
	water.GetTransform().SetScale({ 20.0f, 20.0f, 1.0f });
	water.GetTransform().Rotate(-PI / 2.0f, glm::vec3{ 1, 0, 0 });
	water.GetTransform().SetPosition({ 0, 0, 0 });

	Entity terrainEntity = scene.CreateEntity(DEFAULT_LAYER);
	terrainEntity.AddComponent<ModelRendererComponent>(model);
	terrainEntity.GetTransform().SetPosition({ 0, -10, 0 });

	Entity skybox = scene.CreateEntity(SKYBOX_LAYER);
	skybox.AddComponent<ModelRendererComponent>(Model::Create(GraphicsCache::CubeMesh(), skyboxMaterial));
	skybox.GetTransform().SetPosition({ 0, 0, 0 });
	skybox.GetTransform().SetScale({ 50, 50, 50 });

	Ref<RenderTexture> refractionTexture = RenderTexture::Create(1024, 1024);
	Entity refractionCamera = scene.CreateCamera(camera.GetComponent<CameraComponent>().Frustum);
	refractionCamera.GetComponent<CameraComponent>().ClearColor = SKY_BLUE;
	refractionCamera.GetComponent<CameraComponent>().RenderTarget = *refractionTexture;
	refractionCamera.GetComponent<CameraComponent>().LayerMask = FORGE_LAYERS(DEFAULT_LAYER, SKYBOX_LAYER);
	refractionCamera.GetComponent<CameraComponent>().ClippingPlanes.push_back(glm::vec4{ 0.0f, -1.0f, 0.0f, 0.0f });
	refractionCamera.GetComponent<CameraComponent>().Viewport = { 0, 0, refractionTexture->GetWidth(), refractionTexture->GetHeight() };
	glm::vec4& refractionPlane = refractionCamera.GetComponent<CameraComponent>().ClippingPlanes[0];

	Ref<RenderTexture> reflectionTexture = RenderTexture::Create(1024, 1024);
	Entity reflectionCamera = scene.CreateCamera(camera.GetComponent<CameraComponent>().Frustum);
	reflectionCamera.GetComponent<CameraComponent>().ClearColor = SKY_BLUE;
	reflectionCamera.GetComponent<CameraComponent>().RenderTarget = *reflectionTexture;
	reflectionCamera.GetComponent<CameraComponent>().LayerMask = FORGE_LAYERS(DEFAULT_LAYER, SKYBOX_LAYER);
	reflectionCamera.GetComponent<CameraComponent>().ClippingPlanes.push_back(glm::vec4{ 0.0f, 1.0f, 0.0f, 0.0f });
	reflectionCamera.GetComponent<CameraComponent>().Viewport = { 0, 0, reflectionTexture->GetWidth(), reflectionTexture->GetHeight() };
	glm::vec4& reflectionPlane = reflectionCamera.GetComponent<CameraComponent>().ClippingPlanes[0];

	Ref<Texture2D> normal = Texture2D::Create("res/normal.png");
	Ref<Texture2D> dudv = Texture2D::Create("res/waterDUDV.png");

	waterMaterial->GetUniforms().AddUniform("u_RefractionTexture", refractionTexture);
	waterMaterial->GetUniforms().AddUniform("u_ReflectionTexture", reflectionTexture);
	waterMaterial->GetUniforms().AddUniform("u_DepthTexture", refractionTexture->GetFramebuffer()->GetTexture(ColorBuffer::Depth));
	waterMaterial->GetUniforms().AddUniform("u_NormalMap", normal);
	waterMaterial->GetUniforms().AddUniform("u_DUDVMap", dudv);
	waterMaterial->GetUniforms().AddUniform("u_Time", 0.0f);

	Entity uiCamera = scene.CreateCamera(Frustum::Orthographic(0.0f, app.GetWindow().GetWidth(), 0.0f, app.GetWindow().GetHeight()));
	uiCamera.GetComponent<CameraComponent>().RenderTarget = *screen;
	uiCamera.GetComponent<CameraComponent>().LayerMask = FORGE_LAYERS(TEXTURE_LAYER);
	uiCamera.GetComponent<CameraComponent>().Mode = CameraMode::Overlay;

	int index = 0;
	Ref<Texture2D> textures[] = { refractionTexture, reflectionTexture };
	for (const Ref<Texture2D>& tex : textures)
	{
		Entity e = scene.CreateEntity(TEXTURE_LAYER);
		e.AddComponent<ModelRendererComponent>(Model::Create(GraphicsCache::SquareMesh(), GraphicsCache::DefaultTextureMaterial(tex)));
		e.GetTransform().SetPosition({ 100 + index * 200, 100, -0.5f });
		e.GetTransform().SetScale({ 200, 200, 1 });
		index++;
	}

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
		refractionPlane.y = transform.GetPosition().y < 0 ? 1.0 : -1.0;
		reflectionPlane.y = transform.GetPosition().y < 0 ? -1.0 : 1.0;

		app.OnUpdate();
	}

	return 0;
}

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Forge.h"
using namespace Forge;

#include "Terrain.h"
#include "Ocean.h"

int DEFAULT_LAYER = 0;
int WATER_LAYER = 1;
int SKYBOX_LAYER = 2;

int UI_LAYER = 32;
int TEXTURE_LAYER = 33;

int main()
{
	ForgeInstance::Init();

	WindowProps props;
	props.Title = "Marching Cubes";
	Application app(props);

	ObjReader reader("res/Cruiser.obj");

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
	skyboxMaterial->GetUniforms().SetUniform("u_Texture", skyboxTexture);
	skyboxMaterial->GetSettings().Culling = CullFace::None;

	Scene& scene = app.CreateScene();
	Entity camera = scene.CreateCamera(Frustum::Perspective(PI / 3.0f, app.GetWindow().GetAspectRatio(), 0.01f, 50.0f));
	camera.GetComponent<TransformComponent>().SetLocalPosition({ 0, 2, 10 });
	camera.GetComponent<CameraComponent>().ClearColor = SKY_BLUE;
	camera.GetComponent<CameraComponent>().LayerMask = FORGE_LAYERS(DEFAULT_LAYER, WATER_LAYER, SKYBOX_LAYER);
	camera.GetComponent<CameraComponent>().Priority = 1;
	camera.GetComponent<CameraComponent>().Viewport = { 0, 0, app.GetWindow().GetWidth(), app.GetWindow().GetHeight() };

	Entity sun = scene.CreateEntity();
	sun.GetTransform().Rotate(-PI / 2.0, glm::vec3{ 1, 0, 0 });
	sun.AddComponent<DirectionalLightComponent>();
	sun.GetComponent<DirectionalLightComponent>().Ambient = 0.15f;
	// sun.GetComponent<DirectionalLightComponent>().CreateShadowPass(4096, 4096);
	sun.GetComponent<DirectionalLightComponent>().Shadows.LayerMask = FORGE_LAYERS(DEFAULT_LAYER);
	scene.AddToAllLayers(sun);

	// skyboxMaterial->GetUniforms().SetUniform("u_Texture", sun.GetComponent<LightSourceComponent>().Shadows.RenderTarget->GetDepthAttachment());

	Terrain terrain({ -100, -1450, 300 }, -10.0f);
	Ref<Mesh> mesh = terrain.GenerateMesh({ 20, 20, 20 }, { 200, 200, 200 }, 2.0f);
	Ref<Material> material = Material::CreateFromShaderFile("res/Terrain.shader");
	material->GetUniforms().SetUniform("u_Color", Color{ 112, 72, 60 });
	Ref<Model> model = Model::Create(mesh, material);

	Entity water = scene.CreateEntity(WATER_LAYER);
	Ref<Mesh> waterMesh = GraphicsCache::GridMesh(600, 600);
	water.AddComponent<ModelRendererComponent>(Model::Create(waterMesh, waterMaterial));
	water.GetTransform().SetLocalScale({ 20.0f, 1.0f, 20.0f });
	water.GetTransform().SetLocalPosition({ 0, 0, 0 });
	water.GetComponent<ModelRendererComponent>().Model->GetSubModels()[0].Material->GetSettings().Culling = CullFace::None;

	Entity terrainEntity = scene.CreateEntity(DEFAULT_LAYER);
	terrainEntity.AddComponent<ModelRendererComponent>(model);
	terrainEntity.GetTransform().SetLocalPosition({ 0, -10, 0 });

	Entity skybox = scene.CreateEntity(SKYBOX_LAYER);
	skybox.AddComponent<ModelRendererComponent>(Model::Create(GraphicsCache::CubeMesh(), skyboxMaterial));
	skybox.GetTransform().SetLocalPosition({ 0, 0, 0 });
	skybox.GetTransform().SetLocalScale({ 50, 50, 50 });

	Entity cube = scene.CreateEntity(DEFAULT_LAYER);
	cube.AddComponent<ModelRendererComponent>(Model::Create(reader.GetMesh(), GraphicsCache::LitColorMaterial(COLOR_WHITE)));
	cube.GetTransform().SetLocalPosition({ 0.0f, 0.1f, -1.0f });
	cube.GetTransform().SetLocalScale({ 0.003, 0.003, 0.003 });
	cube.GetTransform().Rotate(PI / 2.0, glm::vec3{ 0, 1, 0 });

	Ref<RenderTexture> refractionTexture = RenderTexture::Create(1024, 1024);
	Entity refractionCamera = scene.CreateCamera(camera.GetComponent<CameraComponent>().Frustum);
	refractionCamera.GetComponent<CameraComponent>().ClearColor = SKY_BLUE;
	refractionCamera.GetComponent<CameraComponent>().RenderTarget = *refractionTexture;
	refractionCamera.GetComponent<CameraComponent>().LayerMask = FORGE_LAYERS(DEFAULT_LAYER, SKYBOX_LAYER);
	refractionCamera.GetComponent<CameraComponent>().ClippingPlanes.push_back(glm::vec4{ 0.0f, -1.0f, 0.0f, 0.05f });
	refractionCamera.GetComponent<CameraComponent>().Viewport = { 0, 0, refractionTexture->GetWidth(), refractionTexture->GetHeight() };
	glm::vec4& refractionPlane = refractionCamera.GetComponent<CameraComponent>().ClippingPlanes[0];

	Ref<RenderTexture> reflectionTexture = RenderTexture::Create(1024, 1024);
	Entity reflectionCamera = scene.CreateCamera(camera.GetComponent<CameraComponent>().Frustum);
	reflectionCamera.GetComponent<CameraComponent>().ClearColor = SKY_BLUE;
	reflectionCamera.GetComponent<CameraComponent>().RenderTarget = *reflectionTexture;
	reflectionCamera.GetComponent<CameraComponent>().LayerMask = FORGE_LAYERS(DEFAULT_LAYER, SKYBOX_LAYER);
	reflectionCamera.GetComponent<CameraComponent>().ClippingPlanes.push_back(glm::vec4{ 0.0f, 1.0f, 0.0f, -0.05f });
	reflectionCamera.GetComponent<CameraComponent>().Viewport = { 0, 0, reflectionTexture->GetWidth(), reflectionTexture->GetHeight() };
	glm::vec4& reflectionPlane = reflectionCamera.GetComponent<CameraComponent>().ClippingPlanes[0];

	Ref<Texture2D> normal = Texture2D::Create("res/normal.png");
	Ref<Texture2D> dudv = Texture2D::Create("res/waterDUDV.png");
	Ref<Texture2D> foam = Texture2D::Create("res/water_foam.png");

	waterMaterial->GetUniforms().SetUniform("u_RefractionTexture", refractionTexture);
	waterMaterial->GetUniforms().SetUniform("u_ReflectionTexture", reflectionTexture);
	waterMaterial->GetUniforms().SetUniform("u_DepthTexture", refractionTexture->GetFramebuffer()->GetDepthAttachment());
	waterMaterial->GetUniforms().SetUniform("u_NormalMap", normal);
	waterMaterial->GetUniforms().SetUniform("u_DUDVMap", dudv);
	// waterMaterial->GetUniforms().SetUniform("u_FoamTexture", foam);
	waterMaterial->GetUniforms().SetUniform("u_Time", 0.0f);
	// waterMaterial->GetSettings().Mode = PolygonMode::Line;

	float time = 0.0f;

	Input::OnKeyPressed.AddEventListener([camera](const KeyCode& key) mutable
	{
		if (key == KeyCode::I)
		{
			glm::vec3 position = camera.GetTransform().GetPosition();
			std::cout << position.x << " " << position.y << " " << position.z << std::endl;
		}
		return false;
	});

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
			float sensitivity = 0.01f;
			glm::vec2 delta = Input::GetRelMousePosition();
			transform.Rotate(-delta.x * sensitivity, glm::vec3{ 0, 1, 0 }, Space::World);
			transform.Rotate(delta.y * sensitivity, glm::vec3{ 1, 0, 0 }, Space::Local);
		}

		refractionCamera.GetTransform().SetFromTransform(transform);
		reflectionCamera.GetTransform().SetFromTransform(transform);
		reflectionCamera.GetTransform().SetLocalPosition({ transform.GetPosition().x, 0.0f - transform.GetPosition().y, transform.GetPosition().z });
		reflectionCamera.GetTransform().FlipX();

		waterMaterial->GetUniforms().SetUniform("u_Time", time);

		refractionPlane.y = transform.GetPosition().y < 0 ? 1.0 : -1.0;
		reflectionPlane.y = transform.GetPosition().y < 0 ? -1.0 : 1.0;

		app.OnUpdate();
	}

	return 0;
}

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Forge.h"

using namespace Forge;

void AddJointEntity(Scene& scene, std::vector<Entity>& jointEntities, const Joint* joint, const glm::mat4& parentTransform, const std::vector<glm::mat4>& jointTransforms)
{
	glm::mat4 transform = parentTransform * jointTransforms[joint->Id];
	glm::vec4 position = (transform)[3];
	Entity entity = scene.CreateEntity();
	entity.AddComponent<ModelRendererComponent>(Model::Create(GraphicsCache::CubeMesh(), GraphicsCache::DefaultColorMaterial(COLOR_WHITE)));
	entity.GetTransform().SetPosition(glm::vec3(position));

	jointEntities[joint->Id] = entity;

	for (const auto& child : joint->Children)
		AddJointEntity(scene, jointEntities, child.get(), transform, jointTransforms);
}

void UpdateJointEntities(std::vector<Entity>& jointEntities, const Joint* joint, const glm::mat4& parentTransform, const std::vector<glm::mat4>& jointTransforms)
{
	glm::mat4 transform = parentTransform * jointTransforms[joint->Id];
	glm::vec4 position = (transform)[3];
	// jointEntities[joint->Id].GetTransform().SetPosition(glm::vec3(position));

	for (const auto& child : joint->Children)
		UpdateJointEntities(jointEntities, child.get(), transform, jointTransforms);
}

int main()
{
	ForgeInstance::Init();

	WindowProps props;
	Application app(props);

	Scene& scene = app.CreateScene();
	Entity camera = scene.CreateCamera(Frustum::Perspective(PI / 3.0f, app.GetWindow().GetAspectRatio(), 0.1f, 1000.0f));
	camera.GetComponent<TransformComponent>().SetPosition({ 0, 0, 10 });

	GltfReader reader("res/Dragon/scene.gltf");
	Ref<Texture2D> texture = Texture2D::Create("res/Dragon/textures/material_0_diffuse.png");

	std::vector<Entity> jointEntities;
	Ref<AnimatedMesh> animatedMesh;
	Entity animatedEntity;

	for (const Ref<Mesh>& mesh : reader.GetMeshes())
	{
		Ref<Material> material;
		if (mesh->IsAnimated())
			material = GraphicsCache::AnimatedLitTextureMaterial(((const Ref<AnimatedMesh>&)mesh)->GetJointCount(), texture);
		else
			material = GraphicsCache::LitTextureMaterial(texture);
		Ref<Model> model = Model::Create(mesh, material);
		Entity entity = scene.CreateEntity();
		entity.AddComponent<ModelRendererComponent>(model);
		if (mesh->IsAnimated())
		{
			entity.AddComponent<AnimatorComponent>().SetCurrentAnimation(reader.GetAnimation("idle"));
			animatedEntity = entity;
			animatedMesh = std::static_pointer_cast<AnimatedMesh>(mesh);
		}
	}

	Entity sun = scene.CreateEntity();
	sun.GetTransform().SetPosition({ 0, 100, 0 });
	sun.AddComponent<LightSourceComponent>();

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

		if (animatedEntity)
		{
			// animatedEntity.GetComponent<AnimatorComponent>().OnUpdate(ts);
		}

		// UpdateJointEntities(jointEntities, &animatedMesh->GetRootJoint(), glm::mat4(1.0f), animatedEntity.GetComponent<AnimatorComponent>().CalculateCurrentPose());

		app.OnUpdate();
	}

	return 0;
}

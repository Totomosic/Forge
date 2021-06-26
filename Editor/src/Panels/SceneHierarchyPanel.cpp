#include "SceneHierarchyPanel.h"
using namespace Forge;
#include "Utils.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Editor
{

	template<typename T, typename FuncT>
	static void DrawComponent(const std::string& name, Entity entity, bool removable, FuncT callback)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			T& component = entity.GetComponent<T>();
			bool removeComponent = false;

			TreeNodeOptions options;
			options.IncludeSeparator = true;
			options.Callback = [&]()
			{
				callback(component);
			};
			if (removable)
			{
				options.OptionsCallback = [&]()
				{
					if (ImGui::MenuItem("Remove component"))
					{
						removeComponent = true;
					}
				};
			}

			DrawTreeNode(name, options);

			if (removeComponent)
			{
				entity.RemoveComponent<T>();
			}
		}
	}

	void SceneHierarchyPanel::SetScene(Scene* scene)
	{
		m_Scene = scene;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		if (m_Scene)
		{
			ImGui::Begin("Scene Hierarchy");

			m_Scene->GetRegistry().each([&](entt::entity entity)
			{
				Entity e(entity, m_Scene);
				DrawEntityNode(e);
			});

			if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
			{
				m_SelectedEntity = {};
			}

			if (ImGui::BeginPopupContextWindow(0, ImGuiMouseButton_Right, false))
			{
				if (ImGui::MenuItem("Create Empty"))
				{
					Entity entity = m_Scene->CreateEntity();
					m_SelectedEntity = entity;
				}
				if (ImGui::MenuItem("Create Point light"))
				{
					Entity entity = m_Scene->CreateEntity();
					entity.AddComponent<PointLightComponent>().CreateShadowPass(DefaultPointShadowMapDimension, DefaultPointShadowMapDimension);
					m_SelectedEntity = entity;
				}
				if (ImGui::MenuItem("Create Directional light"))
				{
					Entity entity = m_Scene->CreateEntity();
					entity.AddComponent<DirectionalLightComponent>();
					entity.GetTransform().Rotate(-PI / 2.0f, { 1, 0, 0 });
					m_SelectedEntity = entity;
				}
				if (ImGui::MenuItem("Create Cube"))
				{
					Entity entity = m_Scene->CreateEntity();
					entity.AddComponent<ModelRendererComponent>(Model::Create(GraphicsCache::CubeMesh(), GraphicsCache::PbrColorMaterial(COLOR_WHITE)));
					m_SelectedEntity = entity;
				}
				if (ImGui::MenuItem("Create Textured Cube"))
				{
					Entity entity = m_Scene->CreateEntity();
					entity.AddComponent<ModelRendererComponent>(Model::Create(GraphicsCache::CubeMesh(), GraphicsCache::LitTextureMaterial(nullptr)));
					m_SelectedEntity = entity;
				}
				if (ImGui::MenuItem("Create Sphere"))
				{
					Entity entity = m_Scene->CreateEntity();
					entity.AddComponent<ModelRendererComponent>(Model::Create(GraphicsCache::SphereMesh(), GraphicsCache::PbrColorMaterial(COLOR_WHITE)));
					m_SelectedEntity = entity;
				}
				if (ImGui::MenuItem("Create Textured Sphere"))
				{
					Entity entity = m_Scene->CreateEntity();
					entity.AddComponent<ModelRendererComponent>(Model::Create(GraphicsCache::SphereMesh(), GraphicsCache::LitTextureMaterial(nullptr)));
					m_SelectedEntity = entity;
				}
				if (ImGui::MenuItem("Create Plane"))
				{
					Entity entity = m_Scene->CreateEntity();
					entity.AddComponent<ModelRendererComponent>(Model::Create(GraphicsCache::GridMesh(2, 2), GraphicsCache::PbrColorMaterial(COLOR_WHITE)));
					m_SelectedEntity = entity;
				}
				ImGui::EndPopup();
			}

			ImGui::End();

			ImGui::Begin("Properties");
			if (m_SelectedEntity)
			{
				DrawComponents(m_SelectedEntity);
			}
			ImGui::End();
		}
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		std::string tag = "<Unnamed>";
		if (entity.HasComponent<TagComponent>())
		{
			tag = entity.GetComponent<TagComponent>().Tag;
		}
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectedEntity = entity;
		}

		bool deleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete"))
			{
				deleted = true;
			}
			if (ImGui::MenuItem("Duplicate"))
			{
				m_Scene->CloneEntity(entity);
			}
			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGui::TreePop();
		}

		if (deleted)
		{
			if (m_SelectedEntity == entity)
				m_SelectedEntity = {};
			m_Scene->DestroyEntity(entity);
		}
	}

	void SceneHierarchyPanel::DrawComponents(Forge::Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			TagComponent& tag = entity.GetComponent<TagComponent>();
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			std::strncpy(buffer, tag.Tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag.Tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("AddComponent");
		}
		
		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::MenuItem("Model renderer"))
			{
				if (!entity.HasComponent<ModelRendererComponent>())
				{
					entity.AddComponent<ModelRendererComponent>(Model::Create(GraphicsCache::CubeMesh(), GraphicsCache::PbrColorMaterial(COLOR_WHITE)));
				}
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Point light"))
			{
				if (!entity.HasComponent<PointLightComponent>())
				{
					entity.AddComponent<PointLightComponent>();
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		bool enabled = entity.Enabled();
		DrawBooleanControl("Enabled", enabled);
		if (enabled != entity.Enabled())
		{
			entity.SetEnabled(enabled);
		}

		DrawComponent<TransformComponent>("Transform", entity, false, [](TransformComponent& transform)
		{
			glm::vec3 position = transform.GetPosition();
			glm::vec3 rotation = glm::degrees(glm::eulerAngles(transform.GetRotation()));
			glm::vec3 scale = transform.GetScale();

			DrawVec3Control("Position", position);
			DrawVec3Control("Rotation", rotation);
			DrawVec3Control("Scale", scale, 1.0f);

			if (position != transform.GetPosition())
				transform.SetPosition(position);
			if (rotation != glm::degrees(glm::eulerAngles(transform.GetRotation())))
				transform.SetRotation(glm::quat(glm::radians(rotation)));
			if (scale != transform.GetScale())
				transform.SetScale(scale);
		});

		DrawComponent<PointLightComponent>("Point light", entity, true, [](PointLightComponent& light)
		{
			DrawColorControl("Color", light.Color);
			DrawFloatControl("Intensity", light.Intensity);
			DrawFloatControl("Ambient", light.Ambient);
			DrawFloatControl("Radius", light.Radius);
			bool shadows = light.Shadows.Enabled;
			DrawBooleanControl("Cast shadows", shadows);
			if (shadows != light.Shadows.Enabled)
			{
				if (shadows)
					light.CreateShadowPass(DefaultShadowMapDimension, DefaultShadowMapDimension);
				else
				{
					light.Shadows.Enabled = false;
					light.Shadows.RenderTarget = nullptr;
				}
			}
		});

		DrawComponent<DirectionalLightComponent>("Directional light", entity, true, [](DirectionalLightComponent& light)
		{
			DrawColorControl("Color", light.Color);
			DrawFloatControl("Intensity", light.Intensity);
			DrawFloatControl("Ambient", light.Ambient);
			bool shadows = light.Shadows.Enabled;
			DrawBooleanControl("Cast shadows", shadows);
			if (shadows != light.Shadows.Enabled)
			{
				if (shadows)
					light.CreateShadowPass(DefaultShadowMapDimension, DefaultShadowMapDimension);
				else
				{
					light.Shadows.Enabled = false;
					light.Shadows.RenderTarget = nullptr;
				}
			}
		});

		DrawComponent<ModelRendererComponent>("Model renderer", entity, true, [](ModelRendererComponent& modelRenderer)
		{
			int index = 0;
			for (Model::SubModel& submodel : modelRenderer.Model->GetSubModels())
			{
				TreeNodeOptions options;
				options.IncludeSeparator = false;
				options.Callback = [&]()
				{
					RenderSettings& settings = submodel.Material->GetSettings();
					bool backFaceCulling = settings.Culling == CullFace::Back;
					bool wireframe = settings.Mode == PolygonMode::Line;
					DrawBooleanControl("Culling", backFaceCulling);
					DrawBooleanControl("Wireframe", wireframe);
					settings.Culling = backFaceCulling ? CullFace::Back : CullFace::None;
					settings.Mode = wireframe ? PolygonMode::Line : PolygonMode::Fill;

					glm::mat4& transform = submodel.Transform;
					glm::vec3 position;
					glm::vec3 scale;
					glm::quat rot;

					Math::DecomposeTransform(transform, position, rot, scale);

					glm::vec3 rotation = glm::degrees(glm::eulerAngles(rot));

					DrawVec3Control("Position", position);
					DrawVec3Control("Rotation", rotation);
					DrawVec3Control("Scale", scale, 1.0f);

					transform = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(glm::quat(glm::radians(rotation))) * glm::scale(glm::mat4(1.0f), scale);
				};
				options.DragDropCallback = [&]()
				{
					const ImGuiPayload* pathPayload = ImGui::AcceptDragDropPayload("PATH");
					if (pathPayload)
					{
						std::string filename = (char*)pathPayload->Data;
						Ref<Mesh> mesh = GraphicsCache::LoadMesh(filename);
						submodel.Mesh = mesh;
					}
					const ImGuiPayload* assetLocationPayload = ImGui::AcceptDragDropPayload("MESH_ASSET_LOCATION_POINTER");
					if (assetLocationPayload)
					{
						const AssetLocation* location = *(const AssetLocation**)assetLocationPayload->Data;
						Ref<Mesh> mesh = GraphicsCache::GetAsset<Mesh>(*location);
						submodel.Mesh = mesh;
					}
				};
				if (submodel.Mesh)
				{
					std::string meshFilename = GraphicsCache::GetAssetLocation(submodel.Mesh).Path;
					DrawTreeNode("Mesh " + std::to_string(index + 1) + " (" + meshFilename + ")", options);
				}
				TreeNodeOptions materialOptions;
				materialOptions.IncludeSeparator = false;
				materialOptions.Callback = [&]()
				{
					UniformContext& uniforms = submodel.Material->GetUniforms();
					for (const UniformSpecification& specification : uniforms.GetUniforms())
					{
						switch (specification.Type)
						{
						case ShaderDataType::Int:
							break;
						case ShaderDataType::Float:
							DrawFloatControl(specification.Name, uniforms.GetUniform<float>(specification.VariableName));
							break;
						case ShaderDataType::Float2:
							DrawVec2Control(specification.Name, uniforms.GetUniform<glm::vec2>(specification.VariableName));
							break;
						case ShaderDataType::Float3:
							DrawVec3Control(specification.Name, uniforms.GetUniform<glm::vec3>(specification.VariableName));
							break;
						case ShaderDataType::Float4:
							DrawColorControl(specification.Name, uniforms.GetUniform<Color>(specification.VariableName));
							break;
						case ShaderDataType::Sampler1D:
						case ShaderDataType::Sampler2D:
						case ShaderDataType::Sampler3D:
						case ShaderDataType::SamplerCube:
							DrawTextureControl(specification.Name, uniforms.GetUniform<Ref<Texture>>(specification.VariableName));
							break;
						}
					}
				};
				materialOptions.DragDropCallback = [&]()
				{
					const ImGuiPayload* pathPayload = ImGui::AcceptDragDropPayload("PATH");
					if (pathPayload)
					{
						std::string filename = (char*)pathPayload->Data;
						Ref<Shader> withoutShadows = GraphicsCache::LoadShader(filename);
						Ref<Shader> withShadows = GraphicsCache::LoadShader(filename, AssetFlags_ShaderShadows);
						submodel.Material = Material::Create(withoutShadows, withShadows);
					}
					const ImGuiPayload* assetLocationPayload = ImGui::AcceptDragDropPayload("SHADER_ASSET_LOCATION_POINTER");
					if (assetLocationPayload)
					{
						const AssetLocation* location = *(const AssetLocation**)assetLocationPayload->Data;
						Ref<Shader> withShadows = GraphicsCache::GetAsset<Shader>(*location);
						submodel.Material = Material::Create(withShadows, withShadows);
					}
				};
				if (submodel.Material)
				{
					std::string shaderFilename = GraphicsCache::GetAssetLocation(submodel.Material->GetShader(RenderPass::WithShadow)).Path;
					DrawTreeNode("Material " + std::to_string(index + 1) + " (" + shaderFilename + ")", materialOptions);
				}
				index++;
			}
		});

		DrawComponent<CameraComponent>("Camera", entity, true, [](CameraComponent& camera)
		{
			DrawColorControl("Clear Color", camera.ClearColor);
		});
	}

}

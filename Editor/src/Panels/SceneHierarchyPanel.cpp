#include "SceneHierarchyPanel.h"
using namespace Forge;

#include <imgui.h>
#include <imgui_internal.h>

namespace Editor
{

	struct TreeNodeOptions
	{
	public:
		bool IncludeSeparator = true;
		std::function<void()> Callback = {};
		std::function<void()> OptionsCallback = {};
	};

	static void DrawBooleanControl(const std::string& name, bool& value, float columnWidth = 100.0f)
	{
		ImGui::PushID(name.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(name.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
		ImGui::Checkbox("##Value", &value);

		ImGui::PopStyleVar();

		ImGui::Columns(1);
		ImGui::PopID();
	}

	static void DrawColorControl(const std::string& name, Color& values, float columnWidth = 100.0f)
	{
		ImGui::PushID(name.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(name.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		ImGui::ColorEdit3("##Value", (float*)&values);

		ImGui::PopStyleVar();

		ImGui::Columns(1);
		ImGui::PopID();
	}

	static void DrawColorControl(const std::string& name, glm::vec4& values, float columnWidth = 100.0f)
	{
		ImGui::PushID(name.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(name.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		ImGui::ColorEdit4("##Value", (float*)&values);

		ImGui::PopStyleVar();

		ImGui::Columns(1);
		ImGui::PopID();
	}

	static void DrawFloatControl(const std::string& name, float& value, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGui::PushID(name.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(name.c_str());
		ImGui::NextColumn();

		ImGui::DragFloat("##Value", &value, 0.01f, 0.0f, 0.0f, "%.2f");

		ImGui::Columns(1);
		ImGui::PopID();
	}

	static void DrawVec2Control(const std::string& name, glm::vec2& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(name.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(name.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	static void DrawVec3Control(const std::string& name, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(name.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(name.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	static void DrawTreeNode(const std::string& name, const TreeNodeOptions& options)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		ImVec2 contentRegion = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		if (options.IncludeSeparator)
			ImGui::Separator();

		bool isOpen = ImGui::TreeNodeEx((void*)name.c_str(), flags, name.c_str());
		ImGui::PopStyleVar();
		if (options.OptionsCallback)
		{
			ImGui::SameLine(contentRegion.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("OptionsMenu");
			}
			if (ImGui::BeginPopup("OptionsMenu"))
			{
				options.OptionsCallback();
				ImGui::EndPopup();
			}
		}

		if (isOpen)
		{
			if (options.Callback)
			{
				options.Callback();
			}
			ImGui::TreePop();
		}
	}

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
				if (ImGui::MenuItem("Create Cube"))
				{
					Entity entity = m_Scene->CreateEntity();
					entity.AddComponent<ModelRendererComponent>(Model::Create(GraphicsCache::CubeMesh(), GraphicsCache::LitColorMaterial(COLOR_WHITE)));
					m_SelectedEntity = entity;
				}
				if (ImGui::MenuItem("Create Plane"))
				{
					Entity entity = m_Scene->CreateEntity();
					entity.AddComponent<ModelRendererComponent>(Model::Create(GraphicsCache::GridMesh(2, 2), GraphicsCache::LitColorMaterial(COLOR_WHITE)));
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
					entity.AddComponent<ModelRendererComponent>(Model::Create(GraphicsCache::CubeMesh(), GraphicsCache::LitColorMaterial(COLOR_WHITE)));
				}
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Light source"))
			{
				if (!entity.HasComponent<LightSourceComponent>())
				{
					entity.AddComponent<LightSourceComponent>();
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
			DrawVec3Control("Scale", scale);

			if (position != transform.GetPosition())
				transform.SetPosition(position);
			if (rotation != glm::degrees(glm::eulerAngles(transform.GetRotation())))
				transform.SetRotation(glm::quat(glm::radians(rotation)));
			if (scale != transform.GetScale())
				transform.SetScale(scale);
		});

		DrawComponent<LightSourceComponent>("Light source", entity, true, [](LightSourceComponent& light)
		{
			DrawColorControl("Color", light.Color);
			DrawFloatControl("Ambient", light.Ambient);
		});

		DrawComponent<ModelRendererComponent>("Model renderer", entity, true, [](ModelRendererComponent& modelRenderer)
		{
			int index = 0;
			for (Model::SubModel& submodel : modelRenderer.Model->GetSubModels())
			{
				TreeNodeOptions options;
				options.IncludeSeparator = true;
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
					DrawVec3Control("Scale", scale);

					transform = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(glm::quat(glm::radians(rotation))) * glm::scale(glm::mat4(1.0f), scale);

					TreeNodeOptions materialOptions;
					materialOptions.IncludeSeparator = false;
					materialOptions.Callback = [&]()
					{
						Shader& shader = *submodel.Material->GetShader(RenderPass::WithoutShadow);
						UniformContext& uniforms = submodel.Material->GetUniforms();
						for (const UniformDescriptor& descriptor : shader.GetUniformDescriptors())
						{
							if (!descriptor.Automatic)
							{
								switch (descriptor.Type)
								{
								case ShaderDataType::Int:
									break;
								case ShaderDataType::Float:
									DrawFloatControl(descriptor.Name, uniforms.GetUniform<float>(descriptor.VariableName));
									break;
								case ShaderDataType::Float2:
									DrawVec2Control(descriptor.Name, uniforms.GetUniform<glm::vec2>(descriptor.VariableName));
									break;
								case ShaderDataType::Float3:
									DrawVec3Control(descriptor.Name, uniforms.GetUniform<glm::vec3>(descriptor.VariableName));
									break;
								case ShaderDataType::Float4:
									DrawColorControl(descriptor.Name, uniforms.GetUniform<Color>(descriptor.VariableName));
									break;
								}
							}
						}
					};

					DrawTreeNode("Material", materialOptions);
				};
				DrawTreeNode(std::string("Mesh " + std::to_string(index + 1)), options);
				index++;
			}
		});

		DrawComponent<CameraComponent>("Camera", entity, true, [](CameraComponent& camera)
		{
			bool shadows = camera.Shadows.Enabled;
			DrawColorControl("Clear Color", camera.ClearColor);
			DrawBooleanControl("Shadows", shadows);
			if (shadows != camera.Shadows.Enabled)
			{
				if (shadows)
					camera.CreateShadowPass(4096, 4096);
				else
					camera.Shadows.Enabled = false;
			}
		});
	}

}

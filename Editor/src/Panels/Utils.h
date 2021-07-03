#pragma once
#include "Forge.h"
#include <functional>

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
		std::function<void()> DragDropCallback = {};
	};

	void DrawBooleanControl(const std::string& name, bool& value, float columnWidth = 100.0f);
	void DrawColorControl(const std::string& name, Forge::Color& values, float columnWidth = 100.0f);
	void DrawColorControl(const std::string& name, glm::vec4& values, float columnWidth = 100.0f);
	void DrawFloatControl(const std::string& name, float& value, float resetValue = 0.0f, float columnWidth = 100.0f);
	void DrawVec2Control(const std::string& name, glm::vec2& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	void DrawVec3Control(const std::string& name, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	void DrawTreeNode(const std::string& name, const TreeNodeOptions& options);

	template<typename T>
	void DrawTextureControl(const std::string& name, Forge::Ref<T>& texture, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGui::PushID(name.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(name.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 2 });
		char buffer[255]{};
		if (texture)
		{
			std::string name = GraphicsCache::GetAssetLocation(texture).Path;
			std::memcpy(buffer, name.c_str(), name.size() + 1);
		}
		ImGui::InputText("##Value", buffer, sizeof(buffer));
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PATH");
			if (payload)
			{
				std::string filename = (char*)payload->Data;
				texture = GraphicsCache::LoadTexture2D(filename);
			}
			const ImGuiPayload* assetLocationPayload = ImGui::AcceptDragDropPayload("TEXTURE_ASSET_LOCATION_POINTER");
			if (assetLocationPayload)
			{
				const AssetLocation* location = *(const AssetLocation**)assetLocationPayload->Data;
				Ref<Texture2D> tex = GraphicsCache::GetAsset<Texture2D>(*location);
				texture = std::reinterpret_pointer_cast<T>(tex);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::PopStyleVar();

		ImGui::Columns(1);
		ImGui::PopID();
	}

}

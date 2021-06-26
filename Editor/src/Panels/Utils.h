#pragma once
#include "Forge.h"
#include <functional>

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
	void DrawTextureControl(const std::string& name, Forge::Ref<Forge::Texture>& texture, float resetValue = 0.0f, float columnWidth = 100.0f);
	void DrawTreeNode(const std::string& name, const TreeNodeOptions& options);

}

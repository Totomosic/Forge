#include "PostProcessingPanel.h"
using namespace Forge;
#include "Utils.h"
#include <imgui.h>

namespace Editor
{

	void PostProcessingPanel::SetRenderer(Renderer3D* renderer)
	{
		m_Renderer = renderer;
	}

	void PostProcessingPanel::OnImGuiRender()
	{
		if (m_Renderer)
		{
			PostProcessor& processor = m_Renderer->GetPostProcessor();
			ImGui::Begin("Post Processing");
			bool enabled = processor.IsEnabled();
			DrawBooleanControl("Enabled", enabled);
			if (enabled != processor.IsEnabled())
			{
				processor.SetEnabled(enabled);
			}
			
			for (const auto& stage : processor.GetStages())
			{
				TreeNodeOptions options;
				options.Callback = [&]()
				{
					bool stageEnabled = stage->IsEnabled();
					DrawBooleanControl("Enabled", stageEnabled);
					if (stageEnabled != stage->IsEnabled())
						stage->SetEnabled(stageEnabled);

					UniformContext& uniforms = stage->GetUniforms();
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
				DrawTreeNode(stage->GetName(), options);
			}

			ImGui::End();
		}
	}

}

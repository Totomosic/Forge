#include "ForgePch.h"
#include "MaterialUniforms.h"

namespace Forge
{

	UniformContext::UniformContext()
		: m_Uniforms()
	{
	}

	void UniformContext::AddFromDescriptor(const UniformDescriptor& descriptor)
	{
		if (!HasUniform(descriptor.VariableName))
		{
			switch (descriptor.Type)
			{
			case ShaderDataType::Int:
				AddUniform(descriptor.VariableName, int(0));
				break;
			case ShaderDataType::Float:
				AddUniform(descriptor.VariableName, 0.0f);
				break;
			case ShaderDataType::Float2:
				AddUniform(descriptor.VariableName, glm::vec2{ 0.0f, 0.0f });
				break;
			case ShaderDataType::Float3:
				AddUniform(descriptor.VariableName, glm::vec3{ 0.0f, 0.0f, 0.0f });
				break;
			case ShaderDataType::Float4:
				AddUniform(descriptor.VariableName, glm::vec4{ 0.0f, 0.0f, 0.0f, 0.0f });
				break;
			}
		}
	}

	void UniformContext::Clear()
	{
		m_Uniforms.clear();
	}

	void UniformContext::Apply(const Ref<Shader>& shader, RendererContext& context) const
	{
		for (const auto& pair : m_Uniforms)
		{
			pair.second->Apply(pair.first, shader, context);
		}
	}

}

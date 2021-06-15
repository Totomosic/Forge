#include "ForgePch.h"
#include "MaterialUniforms.h"
#include "RendererContext.h"

namespace Forge
{

#define FORGE_UNIFORM_REFERENCE(T, Offset) (*(T*)(m_Buffer.get() + (Offset)))

	UniformContext::UniformContext()
		: m_Size(0), m_TextureSize(0), m_UniformSpecificationIndices(), m_UniformSpecifications(), m_Buffer(nullptr), m_Textures(nullptr)
	{
	}

	void UniformContext::CreateFromDescriptors(const std::vector<UniformDescriptor>& descriptors)
	{
		m_Size = 0;
		m_TextureSize = 0;
		m_UniformSpecificationIndices.clear();
		m_UniformSpecifications.clear();
		m_Textures = nullptr;
		for (const UniformDescriptor& descriptor : descriptors)
		{
			if (!descriptor.Automatic)
			{
				UniformSpecification specification;
				specification.Varname = descriptor.VariableName;
				specification.Type = descriptor.Type;
				specification.Offset = m_Size;

				m_UniformSpecificationIndices[specification.Varname] = (int)m_UniformSpecifications.size();
				m_UniformSpecifications.push_back(specification);

				if (specification.Type == ShaderDataType::Sampler1D || specification.Type == ShaderDataType::Sampler2D || specification.Type == ShaderDataType::Sampler3D || specification.Type == ShaderDataType::SamplerCube)
				{
					m_TextureSize++;
				}

				m_Size += GetTypeSize(descriptor.Type);
			}
		}
		m_Buffer = std::make_unique<std::byte[]>(m_Size);
		std::memset(m_Buffer.get(), 0, m_Size);
		if (m_TextureSize > 0)
			m_Textures = std::make_unique<Ref<Texture>[]>(m_TextureSize);
		int index = 0;
		for (const UniformSpecification& specification : m_UniformSpecifications)
		{
			if (specification.Type == ShaderDataType::Sampler1D || specification.Type == ShaderDataType::Sampler2D || specification.Type == ShaderDataType::Sampler3D || specification.Type == ShaderDataType::SamplerCube)
			{
				m_Textures[index] = nullptr;
				FORGE_UNIFORM_REFERENCE(int, specification.Offset) = index;
				index++;
			}
		}
	}

	void UniformContext::Apply(const Ref<Shader>& shader, RendererContext& context) const
	{
		for (const UniformSpecification& specification : m_UniformSpecifications)
		{
			switch (specification.Type)
			{
			case ShaderDataType::Mat4:
				shader->SetUniform(specification.Varname, FORGE_UNIFORM_REFERENCE(glm::mat4, specification.Offset));
				break;
			case ShaderDataType::Mat3:
				shader->SetUniform(specification.Varname, FORGE_UNIFORM_REFERENCE(glm::mat3, specification.Offset));
				break;
			case ShaderDataType::Mat2:
				shader->SetUniform(specification.Varname, FORGE_UNIFORM_REFERENCE(glm::mat2, specification.Offset));
				break;
			case ShaderDataType::Float:
				shader->SetUniform(specification.Varname, FORGE_UNIFORM_REFERENCE(float, specification.Offset));
				break;
			case ShaderDataType::Float2:
				shader->SetUniform(specification.Varname, FORGE_UNIFORM_REFERENCE(glm::vec2, specification.Offset));
				break;
			case ShaderDataType::Float3:
				shader->SetUniform(specification.Varname, FORGE_UNIFORM_REFERENCE(glm::vec3, specification.Offset));
				break;
			case ShaderDataType::Float4:
				shader->SetUniform(specification.Varname, FORGE_UNIFORM_REFERENCE(glm::vec4, specification.Offset));
				break;
			case ShaderDataType::Int:
				shader->SetUniform(specification.Varname, FORGE_UNIFORM_REFERENCE(int, specification.Offset));
				break;
			case ShaderDataType::Bool:
				shader->SetUniform(specification.Varname, FORGE_UNIFORM_REFERENCE(bool, specification.Offset));
				break;
			case ShaderDataType::Sampler1D:
			case ShaderDataType::Sampler2D:
			case ShaderDataType::Sampler3D:
			case ShaderDataType::SamplerCube:
			{
				int textureIndex = FORGE_UNIFORM_REFERENCE(int, specification.Offset);
				int slot = context.BindTexture(m_Textures[textureIndex]);
				shader->SetUniform(specification.Varname, slot);
				break;
			}
			default:
				FORGE_ASSERT(false, "Invalid uniform type");
				break;
			}
		}
	}

#undef FORGE_UNIFORM_REFERENCE

}

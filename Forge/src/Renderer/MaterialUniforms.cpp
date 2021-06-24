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

	void UniformContext::AddFromDescriptors(RenderPass pass, const std::vector<UniformDescriptor>& descriptors)
	{
		for (const UniformDescriptor& descriptor : descriptors)
		{
			if (!descriptor.Automatic)
			{
				if (m_UniformSpecificationIndices.find(descriptor.VariableName) != m_UniformSpecificationIndices.end())
				{
					UniformSpecification& specification = m_UniformSpecifications[m_UniformSpecificationIndices[descriptor.VariableName]];
					FORGE_ASSERT(specification.Type == descriptor.Type, "Avoid uniform names with the same name but different type");
					specification.RenderPasses.set(size_t(pass), true);
				}
				else
				{
					UniformSpecification specification;
					specification.Name = descriptor.Name;
					specification.VariableName = descriptor.VariableName;
					specification.Type = descriptor.Type;
					specification.Offset = m_Size;
					specification.RenderPasses.set(size_t(pass), true);

					m_UniformSpecificationIndices[specification.VariableName] = (int)m_UniformSpecifications.size();
					m_UniformSpecifications.push_back(specification);

					if (specification.Type == ShaderDataType::Sampler1D || specification.Type == ShaderDataType::Sampler2D || specification.Type == ShaderDataType::Sampler3D || specification.Type == ShaderDataType::SamplerCube)
					{
						m_TextureSize++;
					}

					m_Size += GetTypeSize(descriptor.Type);
				}
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
			if (specification.Type == ShaderDataType::Float4)
			{
				FORGE_UNIFORM_REFERENCE(glm::vec4, specification.Offset) = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
			}
		}
	}

	void UniformContext::Apply(RenderPass pass, const Ref<Shader>& shader, RendererContext& context) const
	{
		for (const UniformSpecification& specification : m_UniformSpecifications)
		{
			if (specification.RenderPasses.test(size_t(pass)))
			{
				switch (specification.Type)
				{
				case ShaderDataType::Mat4:
					shader->SetUniform(specification.VariableName, FORGE_UNIFORM_REFERENCE(glm::mat4, specification.Offset));
					break;
				case ShaderDataType::Mat3:
					shader->SetUniform(specification.VariableName, FORGE_UNIFORM_REFERENCE(glm::mat3, specification.Offset));
					break;
				case ShaderDataType::Mat2:
					shader->SetUniform(specification.VariableName, FORGE_UNIFORM_REFERENCE(glm::mat2, specification.Offset));
					break;
				case ShaderDataType::Float:
					shader->SetUniform(specification.VariableName, FORGE_UNIFORM_REFERENCE(float, specification.Offset));
					break;
				case ShaderDataType::Float2:
					shader->SetUniform(specification.VariableName, FORGE_UNIFORM_REFERENCE(glm::vec2, specification.Offset));
					break;
				case ShaderDataType::Float3:
					shader->SetUniform(specification.VariableName, FORGE_UNIFORM_REFERENCE(glm::vec3, specification.Offset));
					break;
				case ShaderDataType::Float4:
					shader->SetUniform(specification.VariableName, FORGE_UNIFORM_REFERENCE(glm::vec4, specification.Offset));
					break;
				case ShaderDataType::Int:
					shader->SetUniform(specification.VariableName, FORGE_UNIFORM_REFERENCE(int, specification.Offset));
					break;
				case ShaderDataType::Bool:
					shader->SetUniform(specification.VariableName, FORGE_UNIFORM_REFERENCE(bool, specification.Offset));
					break;
				case ShaderDataType::Sampler1D:
					ApplyTextureUniform(shader, specification, context, GL_TEXTURE_1D);
					break;
				case ShaderDataType::Sampler2D:
					ApplyTextureUniform(shader, specification, context, GL_TEXTURE_2D);
					break;
				case ShaderDataType::Sampler3D:
					ApplyTextureUniform(shader, specification, context, GL_TEXTURE_3D);
					break;
				case ShaderDataType::SamplerCube:
					ApplyTextureUniform(shader, specification, context, GL_TEXTURE_CUBE_MAP);
					break;
				default:
					FORGE_ASSERT(false, "Invalid uniform type");
					break;
				}
			}
		}
	}

	void UniformContext::ApplyTextureUniform(const Ref<Shader>& shader, const UniformSpecification& specification, RendererContext& context, GLenum textureTarget) const
	{
		int textureIndex = FORGE_UNIFORM_REFERENCE(int, specification.Offset);
		FORGE_ASSERT(!m_Textures[textureIndex] || m_Textures[textureIndex]->GetTarget() == textureTarget, "Invalid texture for uniform");
		int slot = context.BindTexture(m_Textures[textureIndex], textureTarget);
		shader->SetUniform(specification.VariableName, slot);
	}

#undef FORGE_UNIFORM_REFERENCE

}

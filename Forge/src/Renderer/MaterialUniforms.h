#pragma once
#include "Shader.h"
#include "Texture.h"
#include "RenderCommand.h"

namespace Forge
{

	class RendererContext;

	FORGE_API enum class PolygonMode
	{
		Fill = GL_FILL,
		Line = GL_LINE,
	};

	struct FORGE_API RenderSettings
	{
	public:
		PolygonMode Mode = PolygonMode::Fill;
		CullFace Culling = CullFace::Back;
	};

	struct FORGE_API UniformSpecification
	{
	public:
		std::string Varname;
		ShaderDataType Type;
		int Offset;
	};

	class FORGE_API UniformContext
	{
	private:
		int m_Size;
		int m_TextureSize;
		std::unordered_map<std::string, int> m_UniformSpecificationIndices;
		std::vector<UniformSpecification> m_UniformSpecifications;
		std::unique_ptr<std::byte[]> m_Buffer;
		std::unique_ptr<Ref<Texture>[]> m_Textures;

	public:
		UniformContext();

		inline bool HasUniform(const std::string& varname) const { return m_UniformSpecificationIndices.find(varname) != m_UniformSpecificationIndices.end(); }

		template<typename T>
		T& GetUniform(const std::string& varname) const
		{
			FORGE_ASSERT(HasUniform(varname), "Invalid uniform name");
			if constexpr (std::is_same_v<T, Ref<Texture2D>> || std::is_same_v<T, Ref<TextureCube>> || std::is_same_v<T, Ref<RenderTexture>> || std::is_same_v<T, Ref<Texture>>)
			{
				int index = *(int*)(m_Buffer.get() + m_UniformSpecifications[m_UniformSpecificationIndices.at(varname)].Offset);
				return (T&)m_Textures[index];
			}
			return *(T*)(m_Buffer.get() + m_UniformSpecifications[m_UniformSpecificationIndices.at(varname)].Offset);
		}

		template<typename T>
		void SetUniform(const std::string& varname, const T& value) const
		{
			GetUniform<T>(varname) = value;
		}

		void CreateFromDescriptors(const std::vector<UniformDescriptor>& descriptors);
		void Apply(const Ref<Shader>& shader, RendererContext& context) const;

	};

}

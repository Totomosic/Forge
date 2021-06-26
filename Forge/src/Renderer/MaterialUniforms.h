#pragma once
#include "Shader.h"
#include "Texture.h"
#include "RenderCommand.h"

#include <bitset>

namespace Forge
{

	class RendererContext;

	constexpr int RENDER_PASS_COUNT = 5;

	FORGE_API enum class RenderPass
	{
		Pick,
		WithShadow,
		WithoutShadow,
		PointShadowFormation,
		ShadowFormation,
	};

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
		std::string Name;
		std::string VariableName;
		ShaderDataType Type;
		int Offset;
		std::bitset<RENDER_PASS_COUNT> RenderPasses;
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
		UniformContext(const UniformContext& other);
		UniformContext& operator=(const UniformContext& other);
		UniformContext(UniformContext&& other) = default;
		UniformContext& operator=(UniformContext&& other) = default;
		~UniformContext() = default;

		inline const std::vector<UniformSpecification>& GetUniforms() const { return m_UniformSpecifications; }
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

		void AddFromDescriptors(RenderPass pass, const std::vector<UniformDescriptor>& descriptors);
		void Apply(RenderPass pass, const Ref<Shader>& shader, RendererContext& context) const;

	private:
		void ApplyTextureUniform(const Ref<Shader>& shader, const UniformSpecification& specifiation, RendererContext& context, GLenum textureTarget) const;

	};

}

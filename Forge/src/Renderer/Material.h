#pragma once
#include "Shader.h"
#include "Texture.h"
#include "RendererContext.h"

namespace Forge
{

	constexpr const char ShadowMapShaderDefine[] = "SHADOW_MAP";

	class FORGE_API UniformContainerBase
	{
	public:
		virtual ~UniformContainerBase() = default;
		virtual void Apply(const std::string& name, const Ref<Shader>& shader, RendererContext& context) const = 0;
	};

	template<typename T>
	class FORGE_API UniformContainer : public UniformContainerBase
	{
	public:
		T Value;

	public:
		UniformContainer(const T& value) : UniformContainerBase(),
			Value(value)
		{}

		virtual void Apply(const std::string& name, const Ref<Shader>& shader, RendererContext& context) const override
		{
			if constexpr (std::is_same_v<T, Ref<Texture2D>> || std::is_same_v<T, Ref<Texture>> || std::is_same_v<T, Ref<RenderTexture>> || std::is_same_v<T, Ref<TextureCube>>)
			{
				int slot = context.BindTexture(Value);
				shader->SetUniform(name, slot);
			}
			else
				shader->SetUniform(name, Value);
		}
	};

	class FORGE_API UniformContext
	{
	private:
		std::unordered_map<std::string, std::unique_ptr<UniformContainerBase>> m_Uniforms;

	public:
		UniformContext();

		template<typename T>
		void AddUniform(const std::string& name, const T& value)
		{
			m_Uniforms[name] = std::make_unique<UniformContainer<T>>(value);
		}

		template<typename T>
		void UpdateUniform(const std::string& name, const T& value)
		{
			((UniformContainer<T>*)m_Uniforms[name].get())->Value = value;
		}

		void Apply(const Ref<Shader>& shader, RendererContext& context) const;

	};

	class FORGE_API Material
	{
	private:
		std::array<Ref<Shader>, RENDER_PASS_COUNT> m_Shaders;
		UniformContext m_Uniforms;

	public:
		Material();
		Material(const Ref<Shader>& shader);
		Material(std::array<Ref<Shader>, RENDER_PASS_COUNT> shaders);
		virtual ~Material() = default;

		inline const Ref<Shader>& GetShader(RenderPass pass) const { return m_Shaders[int(pass)]; }
		inline const UniformContext& GetUniforms() const { return m_Uniforms; }
		inline UniformContext& GetUniforms() { return m_Uniforms; }
		inline void SetShader(RenderPass pass, const Ref<Shader>& shader) { m_Shaders[int(pass)] = shader; }

		void Apply(RenderPass pass, RendererContext& context) const;

	public:
		static Ref<Material> CreateFromShaderSource(const std::string& vertexSource, const std::string& fragmentSource, ShaderDefines defines = {});
		static Ref<Material> CreateFromShaderFile(const std::string& vertexFilePath, const std::string& fragmentFilePath, ShaderDefines defines = {});
		static Ref<Material> CreateFromShaderFile(const std::string& shaderFilePath, ShaderDefines defines = {});

	};

}

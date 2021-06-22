#pragma once
#include "Shader.h"
#include "Texture.h"
#include "RendererContext.h"

namespace Forge
{

	constexpr const char ShadowMapShaderDefine[] = "SHADOW_MAP";

	struct FORGE_API ShadowFormationShaderSet
	{
	public:
		Ref<Shader> PointShadow = nullptr;
		Ref<Shader> Shadow = nullptr;
	};

	struct FORGE_API MaterialShaderSet
	{
	public:
		Ref<Shader> PickShader = nullptr;
		Ref<Shader> WithShadowShader = nullptr;
		Ref<Shader> WithoutShadowShader = nullptr;
		ShadowFormationShaderSet ShadowFormationShaders = {};
	};

	class FORGE_API Material
	{
	private:
		std::array<Ref<Shader>, RENDER_PASS_COUNT> m_Shaders;
		UniformContext m_Uniforms;
		RenderSettings m_Settings;

	public:
		Material();
		Material(const Ref<Shader>& shader);
		Material(const MaterialShaderSet& shaders);
		virtual ~Material() = default;

		inline const RenderSettings& GetSettings() const { return m_Settings; }
		inline RenderSettings& GetSettings() { return m_Settings; }

		inline const Ref<Shader>& GetShader(RenderPass pass) const { return m_Shaders[int(pass)]; }
		inline const UniformContext& GetUniforms() const { return m_Uniforms; }
		inline UniformContext& GetUniforms() { return m_Uniforms; }
		inline void SetShader(RenderPass pass, const Ref<Shader>& shader) { m_Shaders[int(pass)] = shader; }

		void Apply(RenderPass pass, RendererContext& context) const;

	public:
		static Ref<Material> CreateFromShaderSource(const std::string& vertexSource, const std::string& fragmentSource, ShaderDefines defines = {});
		static Ref<Material> CreateFromShaderFile(const std::string& vertexFilePath, const std::string& fragmentFilePath, ShaderDefines defines = {});
		static Ref<Material> CreateFromShaderSource(const std::string& vertexSource, const std::string& geometrySource, const std::string& fragmentSource, ShaderDefines defines = {});
		static Ref<Material> CreateFromShaderFile(const std::string& vertexFilePath, const std::string& geometryFilePath, const std::string& fragmentFilePath, ShaderDefines defines = {});
		static Ref<Material> CreateFromShaderFile(const std::string& shaderFilePath, ShaderDefines defines = {});
		static Ref<Material> Create(const Ref<Shader>& shader, const Ref<Shader>& shadowShader = nullptr);
		static Ref<Material> Create(const MaterialShaderSet& shaders);

	};

}

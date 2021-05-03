#include "ForgePch.h"
#include "Material.h"
#include "MaterialUniforms.h"

#include "GraphicsCache.h"

namespace Forge
{

	Material::Material() : Material(nullptr)
	{
	}

	Material::Material(const Ref<Shader>& shader) : Material({ GraphicsCache::DefaultPointShadowShader(), shader, shader })
	{
	}

	Material::Material(std::array<Ref<Shader>, RENDER_PASS_COUNT> shaders)
		: m_Shaders(shaders), m_Uniforms()
	{
	}

	void Material::Apply(RenderPass pass, RendererContext& context) const
	{
		m_Uniforms.Apply(GetShader(pass), context);
	}

	Ref<Material> Material::CreateFromShaderSource(const std::string& vertexSource, const std::string& fragmentSource, ShaderDefines defines)
	{
		return CreateFromShaderSource(vertexSource, "", fragmentSource, defines);
	}

	Ref<Material> Material::CreateFromShaderFile(const std::string& vertexFilePath, const std::string& fragmentFilePath, ShaderDefines defines)
	{
		Ref<Shader> shadowFormationShader = GraphicsCache::DefaultPointShadowShader();
		Ref<Shader> withoutShadowShader = Shader::CreateFromFile(vertexFilePath, fragmentFilePath, defines);
		defines.push_back(ShadowMapShaderDefine);
		Ref<Shader> shadowShader = Shader::CreateFromFile(vertexFilePath, fragmentFilePath, defines);
		return CreateRef<Material>(std::array<Ref<Shader>, RENDER_PASS_COUNT>{ shadowFormationShader, shadowShader, withoutShadowShader });
	}

	Ref<Material> Material::CreateFromShaderSource(const std::string& vertexSource, const std::string& geometrySource, const std::string& fragmentSource, ShaderDefines defines)
	{
		Ref<Shader> shadowFormationShader = GraphicsCache::DefaultPointShadowShader();
		Ref<Shader> withoutShadowShader = Shader::CreateFromSource(vertexSource, geometrySource, fragmentSource, defines);
		defines.push_back(ShadowMapShaderDefine);
		Ref<Shader> shadowShader = Shader::CreateFromSource(vertexSource, geometrySource, fragmentSource, defines);
		return CreateRef<Material>(std::array<Ref<Shader>, RENDER_PASS_COUNT>{ shadowFormationShader, shadowShader, withoutShadowShader });
	}

	Ref<Material> Material::CreateFromShaderFile(const std::string& vertexFilePath, const std::string& geometryFilePath, const std::string& fragmentFilePath, ShaderDefines defines)
	{
		Ref<Shader> shadowFormationShader = GraphicsCache::DefaultPointShadowShader();
		Ref<Shader> withoutShadowShader = Shader::CreateFromFile(vertexFilePath, geometryFilePath, fragmentFilePath, defines);
		defines.push_back(ShadowMapShaderDefine);
		Ref<Shader> shadowShader = Shader::CreateFromFile(vertexFilePath, geometryFilePath, fragmentFilePath, defines);
		return CreateRef<Material>(std::array<Ref<Shader>, RENDER_PASS_COUNT>{ shadowFormationShader, shadowShader, withoutShadowShader });
	}

	Ref<Material> Material::CreateFromShaderFile(const std::string& shaderFilePath, ShaderDefines defines)
	{
		Ref<Shader> shadowFormationShader = GraphicsCache::DefaultPointShadowShader();
		Ref<Shader> withoutShadowShader = Shader::CreateFromFile(shaderFilePath, defines);
		defines.push_back(ShadowMapShaderDefine);
		Ref<Shader> shadowShader = Shader::CreateFromFile(shaderFilePath, defines);
		return CreateRef<Material>(std::array<Ref<Shader>, RENDER_PASS_COUNT>{ shadowFormationShader, shadowShader, withoutShadowShader });
	}

}

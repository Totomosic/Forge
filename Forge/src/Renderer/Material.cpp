#include "ForgePch.h"
#include "Material.h"
#include "MaterialUniforms.h"

#include "Assets/GraphicsCache.h"

namespace Forge
{

	Material::Material() : Material(nullptr)
	{
	}

	Material::Material(const Ref<Shader>& shader) : Material(MaterialShaderSet{ GraphicsCache::DefaultPickShader(), shader, shader, GraphicsCache::DefaultShadowShaders() })
	{
	}

	Material::Material(const MaterialShaderSet& shaders)
		: m_Shaders({ shaders.PickShader, shaders.WithShadowShader, shaders.WithoutShadowShader, shaders.ShadowFormationShaders.PointShadow, shaders.ShadowFormationShaders.Shadow }), m_Uniforms()
	{
		m_Uniforms.AddFromDescriptors(RenderPass::PointShadowFormation, GetShader(RenderPass::PointShadowFormation)->GetUniformDescriptors());
		m_Uniforms.AddFromDescriptors(RenderPass::ShadowFormation, GetShader(RenderPass::ShadowFormation)->GetUniformDescriptors());
		m_Uniforms.AddFromDescriptors(RenderPass::WithShadow, GetShader(RenderPass::WithShadow)->GetUniformDescriptors());
		m_Uniforms.AddFromDescriptors(RenderPass::WithoutShadow, GetShader(RenderPass::WithoutShadow)->GetUniformDescriptors());
		m_Uniforms.AddFromDescriptors(RenderPass::Pick, GetShader(RenderPass::Pick)->GetUniformDescriptors());
		m_Uniforms.Init();
	}

	void Material::Apply(RenderPass pass, RendererContext& context) const
	{
		m_Uniforms.Apply(pass, GetShader(pass), context);
	}

	Ref<Material> Material::CreateFromShaderSource(const std::string& vertexSource, const std::string& fragmentSource, ShaderDefines defines)
	{
		return CreateFromShaderSource(vertexSource, "", fragmentSource, defines);
	}

	Ref<Material> Material::CreateFromShaderFile(const std::string& vertexFilePath, const std::string& fragmentFilePath, ShaderDefines defines)
	{
		Ref<Shader> withoutShadowShader = Shader::CreateFromFile(vertexFilePath, fragmentFilePath, defines);
		defines.push_back(ShadowMapShaderDefine);
		Ref<Shader> shadowShader = Shader::CreateFromFile(vertexFilePath, fragmentFilePath, defines);
		return Material::Create(withoutShadowShader, shadowShader);
	}

	Ref<Material> Material::CreateFromShaderSource(const std::string& vertexSource, const std::string& geometrySource, const std::string& fragmentSource, ShaderDefines defines)
	{
		Ref<Shader> withoutShadowShader = Shader::CreateFromSource(vertexSource, geometrySource, fragmentSource, defines);
		defines.push_back(ShadowMapShaderDefine);
		Ref<Shader> shadowShader = Shader::CreateFromSource(vertexSource, geometrySource, fragmentSource, defines);
		return Material::Create(withoutShadowShader, shadowShader);
	}

	Ref<Material> Material::CreateFromShaderFile(const std::string& vertexFilePath, const std::string& geometryFilePath, const std::string& fragmentFilePath, ShaderDefines defines)
	{
		Ref<Shader> withoutShadowShader = Shader::CreateFromFile(vertexFilePath, geometryFilePath, fragmentFilePath, defines);
		defines.push_back(ShadowMapShaderDefine);
		Ref<Shader> shadowShader = Shader::CreateFromFile(vertexFilePath, geometryFilePath, fragmentFilePath, defines);
		return Material::Create(withoutShadowShader, shadowShader);
	}

	Ref<Material> Material::CreateFromShaderFile(const std::string& shaderFilePath, ShaderDefines defines)
	{
		Ref<Shader> withoutShadowShader = Shader::CreateFromFile(shaderFilePath, defines);
		defines.push_back(ShadowMapShaderDefine);
		Ref<Shader> shadowShader = Shader::CreateFromFile(shaderFilePath, defines);
		return Material::Create(withoutShadowShader, shadowShader);
	}

	Ref<Material> Material::Create(const Ref<Shader>& shader, const Ref<Shader>& shadowShader)
	{
		return CreateRef<Material>(MaterialShaderSet{ GraphicsCache::DefaultPickShader(), shadowShader ? shadowShader : shader, shader, GraphicsCache::DefaultShadowShaders() });
	}

	Ref<Material> Material::Create(const MaterialShaderSet& shaders)
	{
		return CreateRef<Material>(shaders);
	}

}

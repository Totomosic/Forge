#include "ForgePch.h"
#include "RendererContext.h"

namespace Forge
{
	void RendererContext::SetShadowMap(const Ref<Texture>& shadowMap)
	{
		if (m_ShadowMap && shadowMap == nullptr)
			m_ShadowMap->Unbind(SHADOW_MAP_TEXTURE_SLOT);
		m_ShadowMap = shadowMap;
		if (shadowMap)
			m_ShadowMap->Bind(SHADOW_MAP_TEXTURE_SLOT);
	}

	void RendererContext::SetCamera(const CameraData& camera)
	{
		m_ProjectionMatrix = camera.ProjectionMatrix;
		m_ViewMatrix = camera.ViewMatrix;
		m_ProjViewMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void RendererContext::SetLightSources(const std::vector<LightSource>& lights)
	{
		m_LightSources = lights;
	}

	void RendererContext::AddLightSource(const LightSource& light)
	{
		m_LightSources.push_back(light);
	}

	void RendererContext::SetClippingPlanes(const std::vector<glm::vec4>& planes)
	{
		m_ClippingPlanes = planes;
	}

	void RendererContext::Reset()
	{
		m_LightSources.clear();
		m_NextTextureSlot = SHADOW_MAP_TEXTURE_SLOT + 1;
	}

	ShaderRequirements RendererContext::GetShaderRequirements(const Ref<Shader>& shader)
	{
		auto it = m_RequirementsMap.find(shader.get());
		if (it != m_RequirementsMap.end())
			return it->second;
		ShaderRequirements requirements;
		requirements.ProjectionMatrix = shader->UniformExists(ProjectionMatrixUniformName);
		requirements.ViewMatrix = shader->UniformExists(ViewMatrixUniformName);
		requirements.ProjViewMatrix = shader->UniformExists(ProjViewMatrixUniformName);
		requirements.ModelMatrix = shader->UniformExists(ModelMatrixUniformName);
		requirements.LightSources = shader->UniformExists(LightSourceArrayUniformName) && shader->UniformExists(UsedLightSourcesUniformName);
		requirements.Animation = shader->UniformExists(JointTransformsUniformName);
		requirements.ClippingPlanes = shader->UniformExists(ClippingPlanesArrayUniformName) && shader->UniformExists(UsedClippingPlanesUniformName);
		requirements.ShadowMap = shader->UniformExists(ShadowMapUniformName) && shader->UniformExists(LightSpaceTransformUniformName);
		m_RequirementsMap[shader.get()] = requirements;
		return requirements;
	}

	void RendererContext::BindShader(const Ref<Shader>& shader, const ShaderRequirements& requirements)
	{
		shader->Bind();
		if (requirements.ViewMatrix)
			shader->SetUniform(ViewMatrixUniformName, m_ViewMatrix);
		if (requirements.ProjectionMatrix)
			shader->SetUniform(ProjectionMatrixUniformName, m_ProjectionMatrix);
		if (requirements.ProjViewMatrix)
			shader->SetUniform(ProjViewMatrixUniformName, m_ProjViewMatrix);
		if (requirements.LightSources)
		{
			shader->SetUniform(UsedLightSourcesUniformName, int(m_LightSources.size()));
			for (size_t i = 0; i < m_LightSources.size(); i++)
			{
				std::string uniformBase = std::string(LightSourceArrayBase) + "[" + std::to_string(i) + "]";
				shader->SetUniform(uniformBase + ".Type", int(m_LightSources[i].Type));
				shader->SetUniform(uniformBase + ".Position", m_LightSources[i].Position);
				shader->SetUniform(uniformBase + ".Direction", m_LightSources[i].Direction);
				shader->SetUniform(uniformBase + ".Attenuation", m_LightSources[i].Attenuation);
				shader->SetUniform(uniformBase + ".Color", m_LightSources[i].Color);
				shader->SetUniform(uniformBase + ".Ambient", m_LightSources[i].Ambient);
			}
		}
		if (requirements.ClippingPlanes)
		{
			shader->SetUniform(UsedClippingPlanesUniformName, int(m_ClippingPlanes.size()));
			for (size_t i = 0; i < m_ClippingPlanes.size(); i++)
			{
				std::string uniformBase = std::string(ClippingPlanesArrayBase) + "[" + std::to_string(i) + "]";
				shader->SetUniform(uniformBase, m_ClippingPlanes[i]);
			}
		}
		if (requirements.ShadowMap)
		{
			shader->SetUniform(ShadowMapUniformName, SHADOW_MAP_TEXTURE_SLOT);
			shader->SetUniform(LightSpaceTransformUniformName, m_LightSpaceTransform);
		}
	}

	int RendererContext::BindTexture(const Ref<Texture>& texture)
	{
		FORGE_ASSERT(m_NextTextureSlot < 32, "Too many textures bound");
		texture->Bind(m_NextTextureSlot);
		return m_NextTextureSlot++;
	}

}

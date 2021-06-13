#include "ForgePch.h"
#include "RendererContext.h"
#include "Material.h"

namespace Forge
{

	RendererContext::RendererContext()
		: m_ViewMatrix(), m_ProjectionMatrix(), m_ProjViewMatrix(), m_CameraFarPlane(0.0f), m_CameraNearPlane(0.0f), m_LightSources(), m_ClippingPlanes(),
		m_NextTextureSlot(0), m_PassUniforms(CreateScope<UniformContext>()), m_CullingEnabled(false), m_RenderSettings(), m_RequirementsMap()
	{
	}

	void RendererContext::SetCamera(const CameraData& camera)
	{
		m_ProjectionMatrix = camera.Frustum.ProjectionMatrix;
		m_ViewMatrix = camera.ViewMatrix;
		m_ProjViewMatrix = m_ProjectionMatrix * m_ViewMatrix;

		m_CameraNearPlane = camera.Frustum.NearPlane;
		m_CameraFarPlane = camera.Frustum.FarPlane;
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
		m_PassUniforms->Clear();
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
		requirements.CameraFarPlane = shader->UniformExists(CameraFarPlaneUniformName);
		requirements.CameraNearPlane = shader->UniformExists(CameraNearPlaneUniformName);
		requirements.CameraPosition = shader->UniformExists(CameraPositionUniformName);
		requirements.LightSources = shader->UniformExists(LightSourceArrayUniformName) && shader->UniformExists(UsedLightSourcesUniformName);
		requirements.Animation = shader->UniformExists(JointTransformsUniformName);
		requirements.ClippingPlanes = shader->UniformExists(ClippingPlanesArrayUniformName) && shader->UniformExists(UsedClippingPlanesUniformName);
		m_RequirementsMap[shader.get()] = requirements;
		return requirements;
	}

	void RendererContext::ApplyRenderSettings(const RenderSettings& settings)
	{
		if (settings.Mode != m_RenderSettings.Mode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, (GLenum)settings.Mode);
			m_RenderSettings.Mode = settings.Mode;
		}
		if ((settings.Culling != m_RenderSettings.Culling && settings.Culling != CullFace::None) || (settings.Culling != CullFace::None && !m_CullingEnabled))
		{
			if (!m_CullingEnabled)
			{
				RenderCommand::EnableCullFace(true);
				m_CullingEnabled = true;
			}
			RenderCommand::SetCullFace(settings.Culling);
			m_RenderSettings.Culling = settings.Culling;
		}
		else if (settings.Culling == CullFace::None && m_CullingEnabled)
		{
			RenderCommand::EnableCullFace(false);
			m_CullingEnabled = false;
		}
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
		if (requirements.CameraFarPlane)
			shader->SetUniform(CameraFarPlaneUniformName, m_CameraFarPlane);
		if (requirements.CameraNearPlane)
			shader->SetUniform(CameraNearPlaneUniformName, m_CameraNearPlane);
		if (requirements.CameraPosition)
			shader->SetUniform(CameraPositionUniformName, m_CameraPosition);
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
		m_PassUniforms->Apply(shader, *this);
	}

	int RendererContext::BindTexture(const Ref<Texture>& texture)
	{
		FORGE_ASSERT(m_NextTextureSlot < 32, "Too many textures bound");
		if (texture)
			texture->Bind(m_NextTextureSlot);
		else
		{
			// TODO: rework
			glActiveTexture(GL_TEXTURE0 + m_NextTextureSlot);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		return m_NextTextureSlot++;
	}

}

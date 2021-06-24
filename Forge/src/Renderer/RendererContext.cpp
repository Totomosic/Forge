#include "ForgePch.h"
#include "RendererContext.h"
#include "Material.h"

namespace Forge
{

	RendererContext::RendererContext()
		: m_ViewMatrix(), m_ProjectionMatrix(), m_ProjViewMatrix(), m_CameraFarPlane(0.0f), m_CameraNearPlane(0.0f), m_CameraPosition(), m_LightSources(), m_ClippingPlanes(),
		m_NextTextureSlot(FirstTextureSlot), m_CullingEnabled(false), m_RenderSettings(), m_RequirementsMap(), m_BoundSlots{ false, false }
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
		for (LightSource& light : m_LightSources)
		{
			GLenum textureTarget = GL_TEXTURE_CUBE_MAP;
			if (light.Type != LightType::Point)
				textureTarget = GL_TEXTURE_2D;
			if (light.ShadowFramebuffer)
			{
				light.ShadowBindLocation = BindTexture(light.ShadowFramebuffer->GetDepthAttachment(), textureTarget);
			}
			else
			{
				light.ShadowBindLocation = BindTexture(nullptr, textureTarget);
			}
		}
	}

	void RendererContext::AddLightSource(const LightSource& light)
	{
		m_LightSources.push_back(light);
	}

	void RendererContext::SetClippingPlanes(const std::vector<glm::vec4>& planes)
	{
		m_ClippingPlanes = planes;
	}

	void RendererContext::SetTime(float time)
	{
		m_Time = time;
	}

	void RendererContext::SetShadowPointMatrices(const glm::vec3& lightPosition, const glm::mat4 matrices[6])
	{
		m_CurrentShadowLightPosition = lightPosition;
		std::memcpy(m_ShadowPointMatrices, matrices, sizeof(glm::mat4) * 6);
	}

	void RendererContext::NewScene()
	{
		m_LightSources.clear();
		m_CurrentShader = nullptr;
	}

	void RendererContext::Reset()
	{
		m_CurrentShader = nullptr;
		m_NextTextureSlot = FirstTextureSlot;
		m_BoundSlots[0] = m_BoundSlots[1] = false;
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
		requirements.Time = shader->UniformExists(TimeUniformName);
		requirements.ShadowFormationLightPosition = shader->UniformExists(ShadowFormationLightPositionUniformName);
		requirements.PointShadowMatrices = shader->UniformExists(PointShadowMatricesArrayUniformName0);
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
		if (m_CurrentShader != shader)
		{
			shader->Bind();
			m_CurrentShader = shader;
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
					glm::vec4 color = { m_LightSources[i].Color.r, m_LightSources[i].Color.g, m_LightSources[i].Color.b, m_LightSources[i].Color.a };
					std::string uniformBase = std::string(LightSourceArrayBase) + "[" + std::to_string(i) + "]";
					shader->SetUniform(uniformBase + ".Type", int(m_LightSources[i].Type));
					shader->SetUniform(uniformBase + ".Position", m_LightSources[i].Position);
					shader->SetUniform(uniformBase + ".Direction", m_LightSources[i].Direction);
					shader->SetUniform(uniformBase + ".Attenuation", m_LightSources[i].Attenuation);
					shader->SetUniform(uniformBase + ".Color", color);
					shader->SetUniform(uniformBase + ".Ambient", m_LightSources[i].Ambient);
					shader->SetUniform(uniformBase + ".Intensity", m_LightSources[i].Intensity);
					shader->SetUniform(uniformBase + ".UseShadows", m_LightSources[i].ShadowFramebuffer != nullptr);
					if (m_LightSources[i].Type == LightType::Point)
					{
						BindTexture(nullptr, GL_TEXTURE_2D);
						shader->SetUniform(uniformBase + ".PointShadowMap", m_LightSources[i].ShadowBindLocation);
						shader->SetUniform(uniformBase + ".ShadowMap", NullTexture2DSlot);
					}
					else
					{
						BindTexture(nullptr, GL_TEXTURE_CUBE_MAP);
						shader->SetUniform(uniformBase + ".ShadowMap", m_LightSources[i].ShadowBindLocation);
						shader->SetUniform(uniformBase + ".PointShadowMap", NullTextureCubeSlot);
					}
					if (m_LightSources[i].ShadowFramebuffer)
					{
						shader->SetUniform(uniformBase + ".ShadowNear", m_LightSources[i].ShadowFrustum.NearPlane);
						shader->SetUniform(uniformBase + ".ShadowFar", m_LightSources[i].ShadowFrustum.FarPlane);
						if (m_LightSources[i].Type != LightType::Point)
						{
							shader->SetUniform(uniformBase + ".LightSpaceTransform", m_LightSources[i].LightSpaceTransform);
						}
					}
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
			if (requirements.Time)
				shader->SetUniform(TimeUniformName, m_Time);
			if (requirements.PointShadowMatrices)
			{
				shader->SetUniform(PointShadowMatricesArrayUniformName0, m_ShadowPointMatrices[0]);
				shader->SetUniform(PointShadowMatricesArrayUniformName1, m_ShadowPointMatrices[1]);
				shader->SetUniform(PointShadowMatricesArrayUniformName2, m_ShadowPointMatrices[2]);
				shader->SetUniform(PointShadowMatricesArrayUniformName3, m_ShadowPointMatrices[3]);
				shader->SetUniform(PointShadowMatricesArrayUniformName4, m_ShadowPointMatrices[4]);
				shader->SetUniform(PointShadowMatricesArrayUniformName5, m_ShadowPointMatrices[5]);
			}
			if (requirements.ShadowFormationLightPosition)
			{
				shader->SetUniform(ShadowFormationLightPositionUniformName, m_CurrentShadowLightPosition);
			}
		}
	}

	int RendererContext::BindTexture(const Ref<Texture>& texture, GLenum textureTarget)
	{
		FORGE_ASSERT(m_NextTextureSlot < 32, "Too many textures bound");
		if (texture)
			texture->Bind(m_NextTextureSlot);
		else
		{
			int slot = textureTarget == GL_TEXTURE_2D ? NullTexture2DSlot : NullTextureCubeSlot;
			if (!m_BoundSlots[slot])
			{
				glActiveTexture(GL_TEXTURE0 + slot);
				glBindTexture(textureTarget, 0);
				m_BoundSlots[slot] = true;
			}
			return slot;
		}
		return m_NextTextureSlot++;
	}

}

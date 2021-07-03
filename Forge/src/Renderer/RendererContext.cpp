#include "ForgePch.h"
#include "RendererContext.h"
#include "Material.h"

namespace Forge
{

	RendererContext::RendererContext()
		: m_NextTextureSlot(FirstTextureSlot), m_NextSceneTextureSlot(FirstTextureSlot), m_CullingEnabled(false), m_RenderSettings(), m_RequirementsMap(), m_BoundSlots{ false, false }
	{
		m_CameraUniformBuffer = UniformBuffer::Create(sizeof(UniformCameraData), CameraDataBindingPoint);
		m_ShadowFormationUniformBuffer = UniformBuffer::Create(sizeof(UniformShadowFormationData), ShadowFormationDataBindingPoint);
		m_ClippingPlaneUniformBuffer = UniformBuffer::Create(sizeof(UniformClippingPlaneData), ClippingPlaneDataBindingPoint);
		m_LightingUniformBuffer = UniformBuffer::Create(sizeof(UniformLightingData), LightingDataBindingPoint);
	}

	void RendererContext::SetCamera(const CameraData& camera)
	{
		UniformCameraData data;
		data.ProjectionMatrix = camera.Frustum.ProjectionMatrix;
		data.ViewMatrix = camera.ViewMatrix;
		data.ProjViewMatrix = data.ProjectionMatrix * data.ViewMatrix;
		data.CameraNearPlane = camera.Frustum.NearPlane;
		data.CameraFarPlane = camera.Frustum.FarPlane;
		data.CameraPosition = glm::inverse(camera.ViewMatrix)[3];
		m_CameraUniformBuffer->SetData(&data, sizeof(UniformCameraData));
	}

	void RendererContext::SetLightSources(const std::vector<LightSource>& lights)
	{
		UniformLightingData data;
		data.UsedLightCount = std::min(int(lights.size()), MAX_LIGHT_COUNT);
		m_LightSourceShadowBindings.resize(data.UsedLightCount);
		for (int i = 0; i < data.UsedLightCount; i++)
		{
			GLenum textureTarget = GL_TEXTURE_CUBE_MAP;
			if (lights[i].Type != LightType::Point)
				textureTarget = GL_TEXTURE_2D;
			data.LightSources[i].Type = int(lights[i].Type);
			data.LightSources[i].Position = lights[i].Position;
			data.LightSources[i].Direction = lights[i].Direction;
			data.LightSources[i].Ambient = lights[i].Ambient;
			data.LightSources[i].Color = { lights[i].Color.r, lights[i].Color.g, lights[i].Color.b, lights[i].Color.a };
			data.LightSources[i].Attenuation = lights[i].Attenuation;
			data.LightSources[i].Intensity = lights[i].Intensity;
			data.LightSources[i].UseShadows = lights[i].ShadowFramebuffer != nullptr;
			if (data.LightSources[i].UseShadows)
			{
				data.LightSources[i].ShadowNear = lights[i].ShadowFrustum.NearPlane;
				data.LightSources[i].ShadowFar = lights[i].ShadowFrustum.FarPlane;
				data.LightSources[i].LightSpaceTransform = lights[i].LightSpaceTransform;

				m_LightSourceShadowBindings[i].Location = BindTexture(lights[i].ShadowFramebuffer->GetDepthAttachment(), textureTarget, true);
				m_LightSourceShadowBindings[i].Type = textureTarget;
			}
			else
			{
				m_LightSourceShadowBindings[i].Location = BindTexture(nullptr, textureTarget, true);
				m_LightSourceShadowBindings[i].Type = textureTarget;
			}
		}
		m_LightingUniformBuffer->SetData(&data, sizeof(UniformLightingData));
	}

	void RendererContext::SetClippingPlanes(const std::vector<glm::vec4>& planes)
	{
		UniformClippingPlaneData data;
		data.UsedClippingPlanes = std::min(int(planes.size()), MAX_CLIPPING_PLANES);
		if (data.UsedClippingPlanes > 0)
			std::memcpy(&data.ClippingPlanes, planes.data(), data.UsedClippingPlanes * sizeof(glm::vec4));
		m_ClippingPlaneUniformBuffer->SetData(&data, sizeof(UniformClippingPlaneData));
	}

	void RendererContext::SetTime(float time)
	{
		m_Time = time;
	}

	void RendererContext::SetShadowPointMatrices(const glm::vec3& lightPosition, const glm::mat4 matrices[6])
	{
		UniformShadowFormationData data;
		data.LightPosition = lightPosition;
		std::memcpy(&data.PointShadowMatrices, matrices, sizeof(glm::mat4) * 6);
		m_ShadowFormationUniformBuffer->SetData(&data, sizeof(UniformShadowFormationData));
	}

	void RendererContext::NewScene()
	{
		m_CurrentShader = nullptr;
	}

	void RendererContext::NewDrawCall()
	{
		m_NextTextureSlot = m_NextSceneTextureSlot;
	}

	void RendererContext::Reset()
	{
		m_CurrentShader = nullptr;
		m_NextTextureSlot = FirstTextureSlot;
		m_NextSceneTextureSlot = FirstTextureSlot;
		m_BoundSlots[0] = m_BoundSlots[1] = false;
	}

	ShaderRequirements RendererContext::GetShaderRequirements(const Ref<Shader>& shader)
	{
		auto it = m_RequirementsMap.find(shader.get());
		if (it != m_RequirementsMap.end())
			return it->second;
		ShaderRequirements requirements;
		requirements.ModelMatrix = shader->UniformExists(ModelMatrixUniformName);
		requirements.LightSourceShadowMaps = shader->UniformExists(LightSourceShadowMapArrayUniformName);
		requirements.Animation = shader->UniformExists(JointTransformsUniformName);
		requirements.Time = shader->UniformExists(TimeUniformName);
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
			if (requirements.LightSourceShadowMaps)
			{
				for (size_t i = 0; i < m_LightSourceShadowBindings.size(); i++)
				{
					std::string uniformBase = std::string(LightSourceShadowMapArrayBase) + '[' + std::to_string(i) + ']';
					if (m_LightSourceShadowBindings[i].Type == GL_TEXTURE_CUBE_MAP)
					{
						shader->SetUniform(uniformBase + ".PointShadowMap", m_LightSourceShadowBindings[i].Location);
						shader->SetUniform(uniformBase + ".ShadowMap", BindTexture(nullptr, GL_TEXTURE_2D));
					}
					else
					{
						shader->SetUniform(uniformBase + ".ShadowMap", m_LightSourceShadowBindings[i].Location);
						shader->SetUniform(uniformBase + ".PointShadowMap", BindTexture(nullptr, GL_TEXTURE_CUBE_MAP));
					}
				}
			}
			if (requirements.Time)
				shader->SetUniform(TimeUniformName, m_Time);
		}
	}

	int RendererContext::BindTexture(const Ref<Texture>& texture, GLenum textureTarget, bool sceneWideTexture)
	{
		FORGE_ASSERT(m_NextTextureSlot < MaxTextureSlots, "Too many textures bound");
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
		if (sceneWideTexture)
			m_NextSceneTextureSlot++;
		return m_NextTextureSlot++;
	}

}

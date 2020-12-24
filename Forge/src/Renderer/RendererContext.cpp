#include "ForgePch.h"
#include "RendererContext.h"

namespace Forge
{

	void RendererContext::SetCamera(const CameraData& camera)
	{
		m_ProjectionMatrix = camera.ProjectionMatrix;
		m_ViewMatrix = camera.ViewMatrix;
		m_ProjViewMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void RendererContext::Reset()
	{
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
	}

}

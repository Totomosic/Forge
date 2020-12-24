#include "ForgePch.h"
#include "Material.h"

namespace Forge
{

	UniformContext::UniformContext()
		: m_Uniforms()
	{
	}

	void UniformContext::Apply(const Ref<Shader>& shader, RendererContext& context) const
	{
		for (const auto& pair : m_Uniforms)
		{
			pair.second->Apply(pair.first, shader, context);
		}
	}

	Material::Material()
		: m_Shader(), m_Uniforms()
	{
	}

	Material::Material(const Ref<Shader>& shader)
		: m_Shader(shader), m_Uniforms()
	{
	}

	void Material::Apply(RendererContext& context) const
	{
		m_Uniforms.Apply(m_Shader, context);
	}

}

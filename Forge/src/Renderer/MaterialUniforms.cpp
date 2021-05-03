#include "ForgePch.h"
#include "MaterialUniforms.h"

namespace Forge
{

	UniformContext::UniformContext()
		: m_Uniforms()
	{
	}

	void UniformContext::Clear()
	{
		m_Uniforms.clear();
	}

	void UniformContext::Apply(const Ref<Shader>& shader, RendererContext& context) const
	{
		for (const auto& pair : m_Uniforms)
		{
			pair.second->Apply(pair.first, shader, context);
		}
	}

}

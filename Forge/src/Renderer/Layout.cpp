#include "ForgePch.h"
#include "Layout.h"

namespace Forge
{

	BufferLayout::BufferLayout(std::initializer_list<VertexAttribute> attributes)
		: m_Attributes(attributes), m_Stride(0)
	{
		Init();
	}

	BufferLayout BufferLayout::Default()
	{
		return BufferLayout({
			{ ShaderDataType::Float3 },
		});
	}

	void BufferLayout::Init()
	{
		m_Stride = 0;
		for (VertexAttribute& attribute : m_Attributes)
		{
			attribute.Offset = m_Stride;
			attribute.GlType = GetGlType(attribute.Type);
			m_Stride += GetTypeSize(attribute.Type);
		}
	}

}

#include "ForgePch.h"
#include "VertexArray.h"

namespace Forge
{

	VertexArray::VertexArray()
		: m_Handle(), m_CurrentAttributeIndex(0), m_VertexBuffers(), m_IndexBuffer(nullptr)
	{
		Init();
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(m_Handle.Id);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& buffer)
	{
		Bind();
		buffer->Bind();

		const BufferLayout& layout = buffer->GetLayout();

		for (const VertexAttribute& attribute : layout)
		{
			glEnableVertexAttribArray(m_CurrentAttributeIndex);
			glVertexAttribPointer(m_CurrentAttributeIndex, GetComponentCount(attribute.Type), attribute.GlType, attribute.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)attribute.Offset);
			m_CurrentAttributeIndex++;
		}

		m_VertexBuffers.push_back(buffer);
	}

	void VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& buffer)
	{
		Bind();
		buffer->Bind();
		m_IndexBuffer = buffer;
	}

	Ref<VertexArray> VertexArray::Create()
	{
		return CreateRef<VertexArray>();
	}

	void VertexArray::Init()
	{
		glCreateVertexArrays(1, &m_Handle.Id);
	}

}

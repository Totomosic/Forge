#include "ForgePch.h"
#include "VertexArray.h"

namespace Forge
{

	constexpr bool IsIntegral(uint32_t glType)
	{
		return glType == GL_INT || glType == GL_UNSIGNED_INT || glType == GL_UNSIGNED_SHORT;
	}

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
		FORGE_ASSERT(m_CurrentAttributeIndex >= 0, "Cannot mix calls to different overloads of AddVertexBuffer");
		Bind();
		buffer->Bind();

		const BufferLayout& layout = buffer->GetLayout();

		for (const VertexAttribute& attribute : layout)
		{
			glEnableVertexAttribArray(m_CurrentAttributeIndex);
			if (IsIntegral(attribute.GlType))
				glVertexAttribIPointer(m_CurrentAttributeIndex, GetComponentCount(attribute.Type), attribute.GlType, layout.GetStride(), (const void*)attribute.Offset);
			else
				glVertexAttribPointer(m_CurrentAttributeIndex, GetComponentCount(attribute.Type), attribute.GlType, attribute.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)attribute.Offset);
			m_CurrentAttributeIndex++;
		}

		m_VertexBuffers.push_back(buffer);
	}

	void VertexArray::AddVertexBuffer(int index, const Ref<VertexBuffer>& buffer)
	{
		m_CurrentAttributeIndex = -1;
		Bind();
		buffer->Bind();

		const BufferLayout& layout = buffer->GetLayout();

		for (const VertexAttribute& attribute : layout)
		{
			glEnableVertexAttribArray(index);
			if (IsIntegral(attribute.GlType))
				glVertexAttribIPointer(index, GetComponentCount(attribute.Type), attribute.GlType, layout.GetStride(), (const void*)attribute.Offset);
			else
				glVertexAttribPointer(index, GetComponentCount(attribute.Type), attribute.GlType, attribute.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)attribute.Offset);
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

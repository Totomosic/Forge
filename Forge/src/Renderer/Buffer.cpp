#include "ForgePch.h"
#include "Buffer.h"

namespace Forge
{

    // ================================================================================
    // VERTEX BUFFER
    // ================================================================================

    VertexBuffer::VertexBuffer() : VertexBuffer(nullptr, 0, BufferLayout::Default())
    {
    }

    VertexBuffer::VertexBuffer(const void* data, size_t sizeBytes, const BufferLayout& layout)
        : m_Handle(), m_Layout(layout)
    {
        Init(data, sizeBytes);
    }

    void VertexBuffer::Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_Handle.Id);
    }

    void VertexBuffer::Unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBuffer::SetData(const void* data, size_t sizeBytes)
    {
        Bind();
        glBufferData(GL_ARRAY_BUFFER, sizeBytes, data, GL_STATIC_DRAW);
    }

    Ref<VertexBuffer> VertexBuffer::Create(size_t sizeBytes, const BufferLayout& layout)
    {
        return CreateRef<VertexBuffer>(nullptr, sizeBytes, layout);
    }

    Ref<VertexBuffer> VertexBuffer::Create(const void* data, size_t sizeBytes, const BufferLayout& layout)
    {
        return CreateRef<VertexBuffer>(data, sizeBytes, layout);
    }

    void VertexBuffer::Init(const void* data, size_t sizeBytes)
    {
        if (sizeBytes > 0)
        {
            glCreateBuffers(1, &m_Handle.Id);
            glBindBuffer(GL_ARRAY_BUFFER, m_Handle.Id);
            glBufferData(GL_ARRAY_BUFFER, sizeBytes, data, GL_STATIC_DRAW);
        }

    }

    // ================================================================================
    // INDEX BUFFER
    // ================================================================================

    IndexBuffer::IndexBuffer() : IndexBuffer(nullptr, 0)
    {
    }

    IndexBuffer::IndexBuffer(const Type* data, size_t sizeBytes, ShaderDataType type)
        : m_Handle(), m_IndexCount(sizeBytes / GetTypeSize(type)), m_GlDataType(GetGlType(type))
    {
        FORGE_ASSERT(type == ShaderDataType::Uint || type == ShaderDataType::Ushort, "Invalid index type");
        Init(data, sizeBytes, type);
    }

    void IndexBuffer::Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle.Id);
    }

    void IndexBuffer::Unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    Ref<IndexBuffer> IndexBuffer::Create(size_t sizeBytes, ShaderDataType type)
    {
        return CreateRef<IndexBuffer>(nullptr, sizeBytes, type);
    }

    Ref<IndexBuffer> IndexBuffer::Create(const Type* data, size_t sizeBytes, ShaderDataType type)
    {
        return CreateRef<IndexBuffer>(data, sizeBytes, type);
    }

    void IndexBuffer::Init(const Type* data, size_t sizeBytes, ShaderDataType type)
    {
        if (sizeBytes > 0)
        {
            glCreateBuffers(1, &m_Handle.Id);
            // Use GL_ARRAY_BUFFER to upload data
            glBindBuffer(GL_ARRAY_BUFFER, m_Handle.Id);
            glBufferData(GL_ARRAY_BUFFER, sizeBytes, data, GL_STATIC_DRAW);
        }
    }

}

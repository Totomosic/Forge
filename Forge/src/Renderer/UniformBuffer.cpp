#include "ForgePch.h"
#include "UniformBuffer.h"

namespace Forge
{

    UniformBuffer::UniformBuffer(uint32_t size, uint32_t binding)
    {
        glCreateBuffers(1, &m_Handle.Id);
        glNamedBufferData(m_Handle.Id, size, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_Handle.Id);
    }

    void UniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
    {
        glNamedBufferSubData(m_Handle.Id, offset, size, data);
    }

    Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
    {
        return CreateRef<UniformBuffer>(size, binding);
    }

}

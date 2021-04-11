#include "ForgePch.h"
#include "Framebuffer.h"

namespace Forge
{

    Framebuffer::Framebuffer(uint32_t width, uint32_t height, int samples)
        : m_Handle(), m_Width(width), m_Height(height), m_Samples(samples), m_Viewport(Viewport{ 0, 0, width, height }), m_TextureBuffers()
    {
    }

    void Framebuffer::Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_Handle.Id);
    }

    void Framebuffer::Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::SetSize(uint32_t width, uint32_t height)
    {
        FORGE_ASSERT(m_Handle.Id == 0, "Not implemented");
        m_Width = width;
        m_Height = height;
    }

    FramebufferTextures Framebuffer::CreateDefaultBuffers()
    {
        return { CreateTextureBuffer(ColorBuffer::Color0), CreateTextureBuffer(ColorBuffer::Depth) };
    }

    Ref<Framebuffer> Framebuffer::Create(uint32_t width, uint32_t height, int samples)
    {
        Ref<Framebuffer> framebuffer = CreateRef<Framebuffer>(width, height, samples);
        framebuffer->Init();
        return framebuffer;
    }

    Ref<Framebuffer> Framebuffer::CreateWindowFramebuffer(uint32_t width, uint32_t height)
    {
        Ref<Framebuffer> framebuffer = CreateRef<Framebuffer>(width, height);
        framebuffer->m_Handle.Id = 0;
        return framebuffer;
    }

    void Framebuffer::CreateColorTextureBuffer(ColorBuffer buffer, const Texture* texture)
    {
        FORGE_ASSERT(m_Handle.Id != 0, "Invalid framebuffer");
        FORGE_ASSERT(!HasTexture(buffer), "Buffer already exists");
        FORGE_ASSERT(!IsMultisampled(), "Multisampled framebuffer cannot have texture attachments");
        Bind();
        texture->Bind();
        texture->SetWrapMode(TextureWrap::ClampToEdge);
        glFramebufferTexture(GL_FRAMEBUFFER, (GLenum)buffer, texture->GetId(), 0);
        glDrawBuffer((GLenum)buffer);
        glReadBuffer((GLenum)buffer);
    }

    void Framebuffer::CreateDepthTextureBuffer(const Texture* texture)
    {
        FORGE_ASSERT(m_Handle.Id != 0, "Invalid framebuffer");
        FORGE_ASSERT(!HasTexture(ColorBuffer::Depth), "Buffer already exists");
        FORGE_ASSERT(!IsMultisampled(), "Multisampled framebuffer cannot have texture attachments");
        Bind();
        texture->Bind();
        texture->SetWrapMode(TextureWrap::ClampToEdge);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture->GetId(), 0);
    }

    void Framebuffer::Init()
    {
        glGenFramebuffers(1, &m_Handle.Id);
        Bind();
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

}

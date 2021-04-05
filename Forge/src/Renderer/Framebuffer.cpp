#include "ForgePch.h"
#include "Framebuffer.h"

namespace Forge
{

    Framebuffer::Framebuffer(uint32_t width, uint32_t height, int samples)
        : m_Handle(), m_Width(width), m_Height(height), m_Samples(samples), m_Viewport(Viewport{ 0, 0, width, height }), m_TextureBuffers(), m_Dirty(false)
    {
    }

    void Framebuffer::Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_Handle.Id);
        m_Dirty = false;
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

    Ref<Texture2D> Framebuffer::CreateTextureBuffer(ColorBuffer buffer, Ref<Texture2D> texture)
    {
        if (!texture)
            texture = Texture2D::Create(GetWidth(), GetHeight());
        if (buffer == ColorBuffer::Depth)
            CreateDepthTextureBuffer(texture.get());
        else
            CreateColorTextureBuffer(buffer, texture.get());
        m_TextureBuffers[buffer] = texture;
        return texture;
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

    void Framebuffer::CreateColorTextureBuffer(ColorBuffer buffer, const Texture2D* texture)
    {
        FORGE_ASSERT(m_Handle.Id != 0, "Invalid framebuffer");
        FORGE_ASSERT(!HasTexture(buffer), "Buffer already exists");
        FORGE_ASSERT(!IsMultisampled(), "Multisampled framebuffer cannot have texture attachments");
        Bind();
        texture->Bind(0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture->GetWidth(), texture->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)buffer, GL_TEXTURE_2D, texture->GetId(), 0);
        glDrawBuffer((GLenum)buffer);
    }

    void Framebuffer::CreateDepthTextureBuffer(const Texture2D* texture)
    {
        FORGE_ASSERT(m_Handle.Id != 0, "Invalid framebuffer");
        FORGE_ASSERT(!HasTexture(ColorBuffer::Depth), "Buffer already exists");
        FORGE_ASSERT(!IsMultisampled(), "Multisampled framebuffer cannot have texture attachments");
        Bind();
        texture->Bind(0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, texture->GetWidth(), texture->GetHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->GetId(), 0);
    }

    void Framebuffer::Init()
    {
        glGenFramebuffers(1, &m_Handle.Id);
    }

}

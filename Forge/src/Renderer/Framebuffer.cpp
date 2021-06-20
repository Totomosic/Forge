#include "ForgePch.h"
#include "Framebuffer.h"

namespace Forge
{

    Ref<Texture2D> CreateTexture2D(FramebufferTextureFormat format, uint32_t width, uint32_t height)
    {
        switch (format)
        {
        case FramebufferTextureFormat::RGBA8:
            return Texture2D::Create(width, height, TextureFormat::RGBA, InternalTextureFormat::RGBA);
        case FramebufferTextureFormat::RED_INTEGER:
            return Texture2D::Create(width, height, TextureFormat::RED_INTEGER, InternalTextureFormat::RED_INTEGER);
        case FramebufferTextureFormat::DEPTH32:
            return Texture2D::Create(width, height, TextureFormat::DEPTH, InternalTextureFormat::DEPTH);
        }
        FORGE_ASSERT(false, "Invalid texture format");
        return nullptr;
    }

    Ref<TextureCube> CreateTextureCube(FramebufferTextureFormat format, uint32_t width, uint32_t height)
    {
        switch (format)
        {
        case FramebufferTextureFormat::RGBA8:
            return TextureCube::Create(width, height, TextureFormat::RGBA, InternalTextureFormat::RGBA);
        case FramebufferTextureFormat::RED_INTEGER:
            return TextureCube::Create(width, height, TextureFormat::RED_INTEGER, InternalTextureFormat::RED_INTEGER);
        case FramebufferTextureFormat::DEPTH32:
            return TextureCube::Create(width, height, TextureFormat::DEPTH, InternalTextureFormat::DEPTH);
        }
        FORGE_ASSERT(false, "Invalid texture format");
        return nullptr;
    }

    Framebuffer::Framebuffer(const FramebufferProps& props)
        : m_Handle(), m_Props(props)
    {
        FORGE_ASSERT(props.Width > 0 && props.Height > 0, "Invalid framebuffer dimensions");
        for (const FramebufferTextureSpecification& format : props.Attachments)
        {
            if (format.TextureFormat == FramebufferTextureFormat::Depth)
                m_DepthAttachmentSpecification = format;
            else
                m_ColorAttachmentSpecifications.push_back(format);
        }
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
        m_Props.Width = width;
        m_Props.Height = height;
        if (m_Handle.Id != 0)
        {
            Invalidate();
        }
    }

    void Framebuffer::ClearAttachment(int index, int value)
    {
        FORGE_ASSERT(index >= 0 && index < m_ColorAttachmentSpecifications.size(), "Invalid attachment index");
        FramebufferTextureSpecification& specification = m_ColorAttachmentSpecifications[index];
        glClearTexImage(m_ColorAttachments[index]->GetId(), 0, FramebufferTextureFormatToOpenGL(specification.TextureFormat), GL_INT, &value);
    }

    int Framebuffer::ReadPixel(int index, int x, int y)
    {
        FORGE_ASSERT(index >= 0 && index < m_ColorAttachmentSpecifications.size(), "Invalid attachment index");
        Bind();
        glReadBuffer(GL_COLOR_ATTACHMENT0 + index);
        int pixelData;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
        return pixelData;
    }

    Ref<Framebuffer> Framebuffer::Create(const FramebufferProps& props)
    {
        Ref<Framebuffer> framebuffer = CreateRef<Framebuffer>(props);
        framebuffer->Invalidate();
        return framebuffer;
    }

    Ref<Framebuffer> Framebuffer::CreateWindowFramebuffer(uint32_t width, uint32_t height)
    {
        FramebufferProps props;
        props.Width = width;
        props.Height = height;
        Ref<Framebuffer> framebuffer = CreateRef<Framebuffer>(props);
        framebuffer->m_Handle.Id = 0;
        return framebuffer;
    }

    void Framebuffer::AttachColorTexture(const Ref<Texture>& texture, int index)
    {
        FORGE_ASSERT(m_Handle.Id != 0, "Invalid framebuffer");
        FORGE_ASSERT(!IsMultisampled(), "Multisampled framebuffer cannot have texture attachments");
        texture->Bind();
        texture->SetWrapMode(TextureWrap::ClampToEdge);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, texture->GetId(), 0);
    }

    void Framebuffer::AttachDepthTexture(const Ref<Texture>& texture)
    {
        FORGE_ASSERT(m_Handle.Id != 0, "Invalid framebuffer");
        FORGE_ASSERT(!IsMultisampled(), "Multisampled framebuffer cannot have texture attachments");
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

    void Framebuffer::Invalidate()
    {
        if (m_Handle.Id != 0)
        {
            glDeleteFramebuffers(1, &m_Handle.Id);
            m_ColorAttachments = {};
            m_DepthAttachment = nullptr;
        }

        glCreateFramebuffers(1, &m_Handle.Id);
        Bind();

        bool multisample = IsMultisampled();

        if (m_ColorAttachmentSpecifications.size() > 0)
        {
            m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());

            for (size_t i = 0; i < m_ColorAttachments.size(); i++)
            {
                switch (m_ColorAttachmentSpecifications[i].TextureType)
                {
                case FramebufferTextureType::Texture2D:
                    m_ColorAttachments[i] = CreateTexture2D(m_ColorAttachmentSpecifications[i].TextureFormat, GetWidth(), GetHeight());
                    break;
                case FramebufferTextureType::TextureCube:
                    m_ColorAttachments[i] = CreateTextureCube(m_ColorAttachmentSpecifications[i].TextureFormat, GetWidth(), GetHeight());
                    break;
                }
                m_ColorAttachments[i]->Bind();
                AttachColorTexture(m_ColorAttachments[i], i);
            }
        }

        if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
        {
            switch (m_DepthAttachmentSpecification.TextureType)
            {
            case FramebufferTextureType::Texture2D:
                m_DepthAttachment = CreateTexture2D(m_DepthAttachmentSpecification.TextureFormat, GetWidth(), GetHeight());
                break;
            case FramebufferTextureType::TextureCube:
                m_DepthAttachment = CreateTextureCube(m_DepthAttachmentSpecification.TextureFormat, GetWidth(), GetHeight());
                break;
            }
            m_DepthAttachment->Bind();
            AttachDepthTexture(m_DepthAttachment);
        }

        if (m_ColorAttachments.size() > 1)
        {
            FORGE_ASSERT(m_ColorAttachments.size() <= 4, "Too many color attachments");
            GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glDrawBuffers(m_ColorAttachments.size(), buffers);
        }
        else if (m_ColorAttachments.empty())
        {
            glDrawBuffer(GL_NONE);
        }

        FORGE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");
    }

}

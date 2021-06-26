#include "ForgePch.h"
#include "PostProcessor.h"
#include "Assets/GraphicsCache.h"
#include "RenderCommand.h"

namespace Forge
{

    PostProcessor::PostProcessor()
        : m_Enabled(false), m_DestinationRenderTarget(), m_ScreenRectangle(), m_StageIndex(0)
    {
        m_ScreenRectangle = GraphicsCache::SquareMesh();
        AddStage(CreateScope<BloomPostProcessingStage>());
        AddStage(CreateScope<HDRPostProcessingStage>());
    }

    void PostProcessor::SetDestination(const Ref<Framebuffer>& renderTarget)
    {
        m_DestinationRenderTarget = renderTarget;
        if (IsEnabled())
        {
            FORGE_ASSERT(GetFirstStage()->GetInputFramebuffer()->SupportsDepth(), "Invalid framebuffer");
            GetFirstStage()->GetInputFramebuffer()->Bind();
        }
        else
        {
            m_DestinationRenderTarget->Bind();
        }
    }

    void PostProcessor::Init(uint32_t width, uint32_t height)
    {
        if (IsEnabled())
        {
            bool first = true;
            for (int i = 0; i < m_Stages.size(); i++)
            {
                if (m_Stages[i]->IsEnabled())
                {
                    m_Stages[i]->Init(width, height, first);
                    first = false;
                }
            }
        }
    }

    void PostProcessor::Render(RendererContext& context)
    {
        if (IsEnabled())
        {
            ResetStages();
            PostProcessingStage* current = GetNextStage();
            while (current != nullptr)
            {
                context.Reset();
                PostProcessingStage* next = GetNextStage();
                if (next != nullptr)
                {
                    current->Execute(next->GetInputFramebuffer(), context);
                }
                else
                {
                    current->Execute(m_DestinationRenderTarget, context);
                }
                current = next;
            }
        }
    }

    void PostProcessor::AddStage(Scope<PostProcessingStage>&& stage)
    {
        stage->SetRenderFunction(std::bind(&PostProcessor::StageRenderFunction, this));
        m_Stages.push_back(std::move(stage));
    }

    void PostProcessor::StageRenderFunction()
    {
        RenderCommand::Clear();
        RenderCommand::DrawIndexed(m_ScreenRectangle->GetDrawMode(), m_ScreenRectangle->GetVertices());
    }

    void PostProcessor::ResetStages()
    {
        m_StageIndex = 0;
    }

    PostProcessingStage* PostProcessor::GetNextStage()
    {
        for (int i = m_StageIndex; i < m_Stages.size(); i++)
        {
            m_StageIndex++;
            if (m_Stages[i]->IsEnabled())
            {
                return m_Stages[i].get();
            }
        }
        return nullptr;
    }

    PostProcessingStage* PostProcessor::GetFirstStage() const
    {
        for (int i = 0; i < m_Stages.size(); i++)
        {
            if (m_Stages[i]->IsEnabled())
            {
                return m_Stages[i].get();
            }
        }
        return nullptr;
    }

    void PostProcessingStage::TestFramebuffer(const Ref<Framebuffer>& framebuffer, uint32_t width, uint32_t height)
    {
        if (framebuffer->GetWidth() != width || framebuffer->GetHeight() != height)
        {
            framebuffer->SetSize(width, height);
        }
    }

    void PostProcessingStage::BindTexture(const Ref<Shader>& shader, const Ref<Texture>& texture, const std::string& uniformName, RendererContext& context)
    {
        int location = context.BindTexture(texture, texture->GetTarget());
        shader->SetUniform(uniformName, location);
    }

    BloomPostProcessingStage::BloomPostProcessingStage()
        : m_FramebufferIndex(0), m_Framebuffers(), m_BloomFramebuffer(), m_BloomShader(), m_BlurShader(), m_BloomCombineShader(), m_Uniforms()
    {
        {
#include "Assets/Shaders/PostProcessing/Bloom.h"
            m_BloomShader = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource);
        }
        {
#include "Assets/Shaders/PostProcessing/Blur.h"
            m_BlurShader = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource);
        }
        {
#include "Assets/Shaders/PostProcessing/BloomCombine.h"
            m_BloomCombineShader = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource);
        }
        m_Uniforms.AddFromDescriptors(PostProcessingRenderPass, m_BloomShader->GetUniformDescriptors());
        m_Uniforms.AddFromDescriptors(PostProcessingRenderPass, m_BlurShader->GetUniformDescriptors());
        m_Uniforms.AddFromDescriptors(PostProcessingRenderPass, m_BloomCombineShader->GetUniformDescriptors());
        m_Uniforms.SetUniform("u_Threshold", 1.0f);
    }

    Ref<Framebuffer> BloomPostProcessingStage::GetInputFramebuffer() const
    {
        return m_Framebuffers[0];
    }

    void BloomPostProcessingStage::Init(uint32_t width, uint32_t height, bool requiresDepth)
    {
        m_FramebufferIndex = 0;
        for (int i = 0; i < 2; i++)
        {
            if (!m_Framebuffers[i] || m_Framebuffers[i]->SupportsDepth() != requiresDepth)
            {
                FramebufferProps props;
                props.Width = width;
                props.Height = height;
                props.Attachments = { FramebufferTextureFormat::RGBA16F };
                if (requiresDepth)
                    props.Attachments.push_back(FramebufferTextureFormat::Depth);
                m_Framebuffers[i] = Framebuffer::Create(props);
            }
            else
            {
                TestFramebuffer(m_Framebuffers[i], width, height);
            }
        }
        if (!m_BloomFramebuffer)
        {
            FramebufferProps props;
            props.Width = width;
            props.Height = height;
            props.Attachments = { FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RGBA16F };
            m_BloomFramebuffer = Framebuffer::Create(props);
        }
        else
        {
            TestFramebuffer(m_BloomFramebuffer, width, height);
        }
    }

    void BloomPostProcessingStage::Execute(const Ref<Framebuffer>& target, RendererContext& context)
    {
        m_BloomShader->Bind();
        BindTexture(m_BloomShader, m_Framebuffers[0]->GetColorAttachment(0), "frg_Texture", context);
        m_Uniforms.Apply(PostProcessingRenderPass, m_BloomShader, context);

        m_BloomFramebuffer->Bind();
        Render();

        bool horizontal = true;
        m_BlurShader->Bind();
        BindTexture(m_BlurShader, m_BloomFramebuffer->GetColorAttachment(1), "frg_Texture", context);

        for (int i = 0; i < 10; i++)
        {
            context.Reset();
            m_Uniforms.SetUniform("u_Horizontal", horizontal);
            m_Uniforms.Apply(PostProcessingRenderPass, m_BlurShader, context);
            m_Framebuffers[horizontal]->Bind();

            Render();
            BindTexture(m_BlurShader, m_Framebuffers[horizontal]->GetColorAttachment(0), "frg_Texture", context);
            horizontal = !horizontal;
        }

        m_BloomCombineShader->Bind();
        BindTexture(m_BloomCombineShader, m_BloomFramebuffer->GetColorAttachment(0), "frg_Texture", context);
        BindTexture(m_BloomCombineShader, m_Framebuffers[!horizontal]->GetColorAttachment(0), "frg_BrightTexture", context);
        m_Uniforms.Apply(PostProcessingRenderPass, m_BloomCombineShader, context);

        target->Bind();
        Render();
    }

    HDRPostProcessingStage::HDRPostProcessingStage()
        : m_Framebuffer(), m_Shader(), m_Uniforms()
    {
        {
#include "Assets/Shaders/PostProcessing/HDR.h"
            m_Shader = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource);
        }
        m_Uniforms.AddFromDescriptors(PostProcessingRenderPass, m_Shader->GetUniformDescriptors());
        m_Uniforms.SetUniform("u_Exposure", 1.0f);
    }

    Ref<Framebuffer> HDRPostProcessingStage::GetInputFramebuffer() const
    {
        return m_Framebuffer;
    }

    void HDRPostProcessingStage::Init(uint32_t width, uint32_t height, bool requiresDepth)
    {
        if (!m_Framebuffer || m_Framebuffer->SupportsDepth() != requiresDepth)
        {
            FramebufferProps props;
            props.Width = width;
            props.Height = height;
            props.Attachments = { FramebufferTextureFormat::RGBA16F };
            if (requiresDepth)
                props.Attachments.push_back(FramebufferTextureFormat::Depth);
            m_Framebuffer = Framebuffer::Create(props);
        }
        else
        {
            TestFramebuffer(m_Framebuffer, width, height);
        }
    }

    void HDRPostProcessingStage::Execute(const Ref<Framebuffer>& target, RendererContext& context)
    {
        m_Shader->Bind();
        BindTexture(m_Shader, m_Framebuffer->GetColorAttachment(0), "frg_Texture", context);
        m_Uniforms.Apply(PostProcessingRenderPass, m_Shader, context);

        target->Bind();
        Render();
    }

}

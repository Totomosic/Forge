#pragma once
#include "Framebuffer.h"
#include "Shader.h"
#include "Mesh.h"
#include "RendererContext.h"
#include "Material.h"

namespace Forge
{

	constexpr RenderPass PostProcessingRenderPass = RenderPass::WithoutShadow;

	class FORGE_API PostProcessingStage
	{
	private:
		std::function<void()> m_RenderFunction{};
		Ref<Texture2D> m_DepthTexture;
		bool m_Enabled = true;

	public:
		virtual ~PostProcessingStage() = default;

		inline bool IsEnabled() const { return m_Enabled; }
		inline void SetEnabled(bool enabled) { m_Enabled = enabled; }
		inline void SetDepthTexture(const Ref<Texture2D>& depthTexture) { m_DepthTexture = depthTexture; }
		inline void SetRenderFunction(const std::function<void()>& fn) { m_RenderFunction = fn; }
		
		virtual const char* GetName() const = 0;
		virtual UniformContext& GetUniforms() = 0;
		virtual Ref<Framebuffer> GetInputFramebuffer() const = 0;
		virtual void Init(uint32_t width, uint32_t height, bool requiresDepth) = 0;
		virtual void Execute(const Ref<Framebuffer>& target, RendererContext& context) = 0;

	protected:
		inline const Ref<Texture2D>& GetDepthTexture() const { return m_DepthTexture; }
		inline void Render() { m_RenderFunction(); };
		void TestFramebuffer(const Ref<Framebuffer>& framebuffer, uint32_t width, uint32_t height);
		void BindTexture(const Ref<Shader>& shader, const Ref<Texture>& texture, const std::string& uniformName, RendererContext& context);
	};

	class FORGE_API BloomPostProcessingStage : public PostProcessingStage
	{
	private:
		int m_FramebufferIndex;
		Ref<Framebuffer> m_Framebuffers[2];
		Ref<Framebuffer> m_BloomFramebuffer;
		Ref<Shader> m_BloomShader;
		Ref<Shader> m_BlurShader;
		Ref<Shader> m_BloomCombineShader;
		UniformContext m_Uniforms;

	public:
		BloomPostProcessingStage();

		inline const char* GetName() const { return "Bloom"; }
		inline virtual UniformContext& GetUniforms() { return m_Uniforms; }
		virtual Ref<Framebuffer> GetInputFramebuffer() const override;
		virtual void Init(uint32_t width, uint32_t height, bool requiresDepth) override;
		virtual void Execute(const Ref<Framebuffer>& target, RendererContext& context) override;
	};

	class FORGE_API HDRPostProcessingStage : public PostProcessingStage
	{
	private:
		Ref<Framebuffer> m_Framebuffer;
		Ref<Shader> m_Shader;
		UniformContext m_Uniforms;

	public:
		HDRPostProcessingStage();

		inline const char* GetName() const { return "HDR"; }
		inline virtual UniformContext& GetUniforms() { return m_Uniforms; }
		virtual Ref<Framebuffer> GetInputFramebuffer() const override;
		virtual void Init(uint32_t width, uint32_t height, bool requiresDepth) override;
		virtual void Execute(const Ref<Framebuffer>& target, RendererContext& context) override;
	};

	class FORGE_API DitherPostProcessingStage : public PostProcessingStage
	{
	private:
		Ref<Framebuffer> m_Framebuffer;
		Ref<Shader> m_Shader;
		Ref<Texture2D> m_BayerMatrix;
		UniformContext m_Uniforms;

	public:
		DitherPostProcessingStage();

		inline const char* GetName() const { return "Dither"; }
		inline virtual UniformContext& GetUniforms() { return m_Uniforms; }
		virtual Ref<Framebuffer> GetInputFramebuffer() const override;
		virtual void Init(uint32_t width, uint32_t height, bool requiresDepth) override;
		virtual void Execute(const Ref<Framebuffer>& target, RendererContext& context) override;
	};

	class FORGE_API PostProcessor
	{
	private:
		bool m_Enabled;
		Ref<Framebuffer> m_DestinationRenderTarget;
		std::vector<Scope<PostProcessingStage>> m_Stages;
		Ref<Mesh> m_ScreenRectangle;

		int m_StageIndex;

	public:
		PostProcessor();

		inline bool IsEnabled() const { return m_Enabled && GetFirstStage() != nullptr; }
		inline void SetEnabled(bool enabled) { m_Enabled = enabled; }
		inline const std::vector<Scope<PostProcessingStage>>& GetStages() const { return m_Stages; }

		void SetDestination(const Ref<Framebuffer>& renderTarget);

		void Init(uint32_t width, uint32_t height);
		void Render(RendererContext& context);

	private:
		void AddStage(Scope<PostProcessingStage>&& stage);
		void StageRenderFunction();

		void ResetStages();
		PostProcessingStage* GetNextStage();
		PostProcessingStage* GetFirstStage() const;
	};

}

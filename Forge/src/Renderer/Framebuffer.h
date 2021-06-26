#pragma once
#include "ForgePch.h"
#include "Texture.h"
#include "Core/Viewport.h"

#include <glad/glad.h>

namespace Forge
{

	namespace Detail
	{
	
		struct FORGE_API FramebufferDestructor
		{
		public:
			inline void operator()(uint32_t id) const
			{
				if (id != 0)
					glDeleteFramebuffers(1, &id);
			}
		};

	}

	enum class FramebufferTextureFormat
	{
		None = 0,

		RGBA8,
		RGBA16F,
		RED_INTEGER,

		DEPTH32,
		Depth = DEPTH32,
	};

	enum class FramebufferTextureType
	{
		Texture2D,
		TextureCube,
	};

	inline constexpr GLenum FramebufferTextureFormatToOpenGL(FramebufferTextureFormat format)
	{
		switch (format)
		{
		case FramebufferTextureFormat::RGBA8:
			return GL_RGBA8;
		case FramebufferTextureFormat::RGBA16F:
			return GL_RGBA16F;
		case FramebufferTextureFormat::RED_INTEGER:
			return GL_RED_INTEGER;
		}
		FORGE_ASSERT(false, "Invalid texture format");
		return 0;
	}

	struct FORGE_API FramebufferTextureSpecification
	{
	public:
		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		FramebufferTextureType TextureType = FramebufferTextureType::Texture2D;

		FramebufferTextureSpecification() = default;
		inline FramebufferTextureSpecification(FramebufferTextureFormat format, FramebufferTextureType type = FramebufferTextureType::Texture2D)
			: TextureFormat(format), TextureType(type)
		{}
	};

	struct FORGE_API FramebufferProps
	{
	public:
		uint32_t Width = 1280;
		uint32_t Height = 720;
		int Samples = 0;
		std::vector<FramebufferTextureSpecification> Attachments;
	};

	class FORGE_API Framebuffer
	{
	private:
		using Handle = Detail::ScopedId<Detail::FramebufferDestructor>;

		Handle m_Handle;
		FramebufferProps m_Props;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

		std::vector<Ref<Texture>> m_ColorAttachments;
		Ref<Texture> m_DepthAttachment;

	public:
		Framebuffer(const FramebufferProps& props);

		inline bool SupportsDepth() const { return m_DepthAttachment != nullptr; }
		inline uint32_t GetWidth() const { return m_Props.Width; }
		inline uint32_t GetHeight() const { return m_Props.Height; }
		inline int GetSamples() const { return m_Props.Samples; }
		inline float GetAspect() const { return float(GetWidth()) / float(GetHeight()); }
		inline bool IsMultisampled() const { return GetSamples() > 0; }
		inline Viewport GetViewport() const { return { 0, 0, GetWidth(), GetHeight() }; }

		inline Ref<Texture> GetColorAttachment(int index) const { FORGE_ASSERT(index >= 0 && index < m_ColorAttachments.size(), "Invalid index"); return m_ColorAttachments[index]; }
		inline Ref<Texture> GetDepthAttachment() const { return m_DepthAttachment; }

		void Bind() const;
		void Unbind() const;
		void SetSize(uint32_t width, uint32_t height);
		void ClearAttachment(int index, int value);
		int ReadPixel(int index, int x, int y);

		friend class RenderTexture;

	public:
		static Ref<Framebuffer> Create(const FramebufferProps& props);
		static Ref<Framebuffer> CreateWindowFramebuffer(uint32_t width, uint32_t height);

	private:
		void AttachColorTexture(const Ref<Texture>& texture, int index);
		void AttachDepthTexture(const Ref<Texture>& texture);
		void Init();
		void Invalidate();

	};

}

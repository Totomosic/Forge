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

	enum class ColorBuffer
	{
		Color0 = GL_COLOR_ATTACHMENT0,
		Color1 = GL_COLOR_ATTACHMENT1,
		Color2 = GL_COLOR_ATTACHMENT2,
		Color3 = GL_COLOR_ATTACHMENT3,
		Color4 = GL_COLOR_ATTACHMENT4,
		Color5 = GL_COLOR_ATTACHMENT5,
		Depth = GL_DEPTH_ATTACHMENT,
	};

	struct FORGE_API FramebufferTextures
	{
	public:
		Ref<Texture2D> Color;
		Ref<Texture2D> Depth;
	};

	class FORGE_API Framebuffer
	{
	private:
		using Handle = Detail::ScopedId<Detail::FramebufferDestructor>;

		Handle m_Handle;
		uint32_t m_Width;
		uint32_t m_Height;
		int m_Samples;
		Viewport m_Viewport;
		std::unordered_map<ColorBuffer, Ref<Texture2D>> m_TextureBuffers;

	public:
		Framebuffer(uint32_t width, uint32_t height, int samples = 0);

		inline uint32_t GetWidth() const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height; }
		inline int GetSamples() const { return m_Samples; }
		inline float GetAspect() const { return float(GetWidth()) / float(GetHeight()); }
		inline bool IsMultisampled() const { return GetSamples() > 0; }
		inline bool HasTexture(ColorBuffer buffer) const { return m_TextureBuffers.find(buffer) != m_TextureBuffers.end(); }
		inline Ref<Texture2D> GetTexture(ColorBuffer buffer) const { return m_TextureBuffers.at(buffer); }

		void Bind() const;
		void Unbind() const;
		void SetSize(uint32_t width, uint32_t height);

		Ref<Texture2D> CreateTextureBuffer(ColorBuffer buffer = ColorBuffer::Color0, Ref<Texture2D> texture = nullptr);
		FramebufferTextures CreateDefaultBuffers();

		friend class RenderTexture;

	public:
		static Ref<Framebuffer> Create(uint32_t width, uint32_t height, int samples = 0);
		static Ref<Framebuffer> CreateWindowFramebuffer(uint32_t width, uint32_t height);

	private:
		void CreateColorTextureBuffer(ColorBuffer buffer, const Texture2D* texture);
		void CreateDepthTextureBuffer(const Texture2D* texture);
		void Init();

	};

}

#pragma once
#include "ForgePch.h"
#include "Buffer.h"

#include <glad/glad.h>

namespace Forge
{

	class Framebuffer;

	namespace Detail
	{

		struct FORGE_API TextureDestructor
		{
		public:
			inline void operator()(uint32_t id) const
			{
				glDeleteTextures(1, &id);
			}
		};

	}

	FORGE_API enum class TextureFormat
	{
		RED = GL_RED,
		RGB = GL_RGB,
		RGBA = GL_RGBA,
		BGRA = GL_BGRA,
		BGR = GL_BGR,
	};

	class FORGE_API Texture
	{
	protected:
		using Handle = Detail::ScopedId<Detail::TextureDestructor>;

		Handle m_Handle;
		uint32_t m_Width;
		uint32_t m_Height;

	public:
		inline Texture(uint32_t width, uint32_t height)
			: m_Handle(), m_Width(width), m_Height(height)
		{}
		virtual ~Texture() = default;

		inline uint32_t GetId() const { return m_Handle.Id; }
		inline uint32_t GetWidth() const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height; }

		virtual void Bind(int slot) const = 0;
		virtual void Unbind(int slot) const = 0;

	};

	class FORGE_API Texture2D : public Texture
	{
	private:
		TextureFormat m_Format;
		bool m_HasMipmaps;

	public:
		Texture2D(uint32_t width, uint32_t height);
		Texture2D(const void* pixels, uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA);

		inline TextureFormat GetFormat() const { return m_Format; }

		virtual void Bind(int slot) const override;
		virtual void Unbind(int slot) const override;

		void GenerateMipmaps();

	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const std::string& filename);

	private:
		void Init(const void* data, uint32_t width, uint32_t height, uint32_t format);

	};

	class FORGE_API TextureCube : public Texture
	{
	private:
		TextureFormat m_Format;
		bool m_HasMipmaps;

	public:
		TextureCube(uint32_t width, uint32_t height);
		TextureCube(const void** pixels, uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA);

		virtual void Bind(int slot) const override;
		virtual void Unbind(int slot) const override;

		void GenerateMipmaps();

	public:
		static Ref<TextureCube> Create(
			const std::string& front,
			const std::string& back,
			const std::string& left,
			const std::string& right,
			const std::string& bottom,
			const std::string& top
		);

	private:
		void Init(const void** data, uint32_t width, uint32_t height, uint32_t format);
	};

	enum class TextureComponent
	{
		Color,
		Depth,
	};

	class FORGE_API RenderTexture : public Texture2D
	{
	private:
		Ref<Framebuffer> m_Framebuffer;

	public:
		RenderTexture(uint32_t width, uint32_t height, TextureComponent component = TextureComponent::Color);

		inline const Ref<Framebuffer>& GetFramebuffer() const { return m_Framebuffer; }

	public:
		static Ref<RenderTexture> Create(uint32_t width, uint32_t height, TextureComponent component = TextureComponent::Color);

	};

}

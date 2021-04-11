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
		DEPTH = GL_DEPTH_COMPONENT,
	};

	FORGE_API enum class InternalTextureFormat
	{
		RGBA = GL_RGBA,
		DEPTH = GL_DEPTH_COMPONENT32,
	};

	FORGE_API enum class TextureFilter
	{
		Nearest,
		Linear,
	};

	FORGE_API enum class TextureWrap
	{
		Repeat = GL_REPEAT,
		ClampToEdge = GL_CLAMP_TO_EDGE,
		ClampToBorder = GL_CLAMP_TO_BORDER,
	};

	class FORGE_API Texture
	{
	protected:
		using Handle = Detail::ScopedId<Detail::TextureDestructor>;

		Handle m_Handle;
		uint32_t m_Width;
		uint32_t m_Height;
		TextureFormat m_Format;
		InternalTextureFormat m_InternalFormat;
		bool m_HasMipmaps;

		mutable TextureFilter m_MinFilter;
		mutable TextureFilter m_MagFilter;
		mutable TextureWrap m_WrapMode;
		GLenum m_Target;

	public:
		inline Texture(GLenum target, uint32_t width, uint32_t height, TextureFormat format, InternalTextureFormat internalFormat)
			: m_Handle(), m_Width(width), m_Height(height), m_Target(target), m_Format(format), m_InternalFormat(internalFormat), m_HasMipmaps(false),
			m_MinFilter(), m_MagFilter(), m_WrapMode()
		{}
		virtual ~Texture() = default;

		inline GLenum GetTarget() const { return m_Target; }
		inline uint32_t GetId() const { return m_Handle.Id; }
		inline uint32_t GetWidth() const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height; }
		inline TextureFormat GetFormat() const { return m_Format; }

		void Bind() const;
		void Unbind() const;
		void Bind(int slot) const;
		void Unbind(int slot) const;

		void GenerateMipmaps();

		void SetMinFilter(TextureFilter filter) const;
		void SetMagFilter(TextureFilter filter) const;
		void SetWrapMode(TextureWrap mode) const;

	protected:
		TextureFilter GetDefaultFilter() const;
		GLenum GetComponentType() const;

	};

	class FORGE_API Texture2D : public Texture
	{
	public:
		Texture2D(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA, InternalTextureFormat internalFormat = InternalTextureFormat::RGBA);
		Texture2D(const void* pixels, uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA, InternalTextureFormat internalFormat = InternalTextureFormat::RGBA);

	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA, InternalTextureFormat internalFormat = InternalTextureFormat::RGBA);
		static Ref<Texture2D> Create(const std::string& filename);

	private:
		void Init(const void* data, uint32_t width, uint32_t height, uint32_t format);

	};

	class FORGE_API TextureCube : public Texture
	{
	public:
		TextureCube(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA, InternalTextureFormat internalFormat = InternalTextureFormat::RGBA);
		TextureCube(const void** pixels, uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA, InternalTextureFormat internalFormat = InternalTextureFormat::RGBA);

	public:
		static Ref<TextureCube> Create(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA, InternalTextureFormat internalFormat = InternalTextureFormat::RGBA);

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

		inline operator Ref<Framebuffer>() const { return m_Framebuffer; }
		inline const Ref<Framebuffer>& GetFramebuffer() const { return m_Framebuffer; }

	public:
		static Ref<RenderTexture> Create(uint32_t width, uint32_t height, TextureComponent component = TextureComponent::Color);

	};

}

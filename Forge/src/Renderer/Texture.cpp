#include "ForgePch.h"
#include "Texture.h"
#include "Framebuffer.h"

#include <stb_image.h>

namespace Forge
{

	void Texture::Bind() const
	{
		glBindTexture(m_Target, GetId());
	}

	void Texture::Unbind() const
	{
		glBindTexture(m_Target, 0);
	}

	void Texture::Bind(int slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(m_Target, GetId());
	}

	void Texture::Unbind(int slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(m_Target, 0);
	}

	void Texture::GenerateMipmaps()
	{
		m_HasMipmaps = true;
		Bind();
		glGenerateMipmap(m_Target);
		SetMinFilter(m_MinFilter);
	}

	void Texture::SetMinFilter(TextureFilter filter) const
	{
		m_MinFilter = filter;
		glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, filter == TextureFilter::Linear ? (m_HasMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR) : (m_HasMipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST));
	}

	void Texture::SetMagFilter(TextureFilter filter) const
	{
		m_MagFilter = filter;
		glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST);
	}

	void Texture::SetWrapMode(TextureWrap mode) const
	{
		m_WrapMode = mode;
		glTexParameteri(m_Target, GL_TEXTURE_WRAP_R, GLenum(mode));
		glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GLenum(mode));
		glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GLenum(mode));
	}

	TextureFilter Texture::GetDefaultFilter() const
	{
		return m_InternalFormat == InternalTextureFormat::DEPTH ? TextureFilter::Nearest : TextureFilter::Linear;
	}

	GLenum Texture::GetComponentType() const
	{
		return m_InternalFormat == InternalTextureFormat::DEPTH ? GL_FLOAT : GL_UNSIGNED_BYTE;
	}

	Texture2D::Texture2D(uint32_t width, uint32_t height, TextureFormat format, InternalTextureFormat internalFormat) : Texture2D(nullptr, width, height, format, internalFormat)
	{
	}

	Texture2D::Texture2D(const void* pixels, uint32_t width, uint32_t height, TextureFormat format, InternalTextureFormat internalFormat) : Texture(GL_TEXTURE_2D, width, height, format, internalFormat)
	{
		Init(pixels, width, height, uint32_t(format));
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, TextureFormat format, InternalTextureFormat internalFormat)
	{
		return CreateRef<Texture2D>(width, height, format, internalFormat);
	}

	Ref<Texture2D> Texture2D::Create(const std::string& filename)
	{
		int width;
		int height;
		int channels;

		unsigned char* pixels = stbi_load(filename.c_str(), &width, &height, &channels, 4);

		Ref<Texture2D> texture = CreateRef<Texture2D>((const void*)pixels, uint32_t(width), uint32_t(height), TextureFormat::RGBA);
		stbi_image_free(pixels);
		return texture;
	}

	void Texture2D::Init(const void* data, uint32_t width, uint32_t height, uint32_t format)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle.Id);
		Bind();
		glTexImage2D(GL_TEXTURE_2D, 0, GLenum(m_InternalFormat), width, height, 0, format, GetComponentType(), data);
		SetMinFilter(GetDefaultFilter());
		SetMagFilter(GetDefaultFilter());
		SetWrapMode(TextureWrap::Repeat);
	}

	TextureCube::TextureCube(uint32_t width, uint32_t height, TextureFormat format, InternalTextureFormat internalFormat) : TextureCube(nullptr, width, height, format, internalFormat)
	{
	}

	TextureCube::TextureCube(const void** pixels, uint32_t width, uint32_t height, TextureFormat format, InternalTextureFormat internalFormat) : Texture(GL_TEXTURE_CUBE_MAP, width, height, format, internalFormat)
	{
		Init(pixels, width, height, uint32_t(format));
	}

	Ref<TextureCube> TextureCube::Create(const std::string& front, const std::string& back, const std::string& left, const std::string& right, const std::string& bottom, const std::string& top)
	{
		int width;
		int height;
		int components;

		void* pixels[6];
		pixels[0] = stbi_load(front.c_str(), &width, &height, &components, 4);
		int originalWidth = width;
		int originalHeight = height;
		pixels[1] = stbi_load(back.c_str(), &width, &height, &components, 4);
		FORGE_ASSERT(width == originalWidth && height == originalHeight, "All images must have identical pixel dimensions");
		pixels[2] = stbi_load(left.c_str(), &width, &height, &components, 4);
		FORGE_ASSERT(width == originalWidth && height == originalHeight, "All images must have identical pixel dimensions");
		pixels[3] = stbi_load(right.c_str(), &width, &height, &components, 4);
		FORGE_ASSERT(width == originalWidth && height == originalHeight, "All images must have identical pixel dimensions");
		pixels[4] = stbi_load(bottom.c_str(), &width, &height, &components, 4);
		FORGE_ASSERT(width == originalWidth && height == originalHeight, "All images must have identical pixel dimensions");
		pixels[5] = stbi_load(top.c_str(), &width, &height, &components, 4);
		FORGE_ASSERT(width == originalWidth && height == originalHeight, "All images must have identical pixel dimensions");

		Ref<TextureCube> texture = CreateRef<TextureCube>((const void**)pixels, uint32_t(width), uint32_t(height));
		for (auto pixel : pixels)
			stbi_image_free(pixel);
		return texture;
	}

	void TextureCube::Init(const void** data, uint32_t width, uint32_t height, uint32_t format)
	{
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_Handle.Id);
		Bind();
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GLenum(m_InternalFormat), width, height, 0, GLenum(format), GetComponentType(), data ? data[0] : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GLenum(m_InternalFormat), width, height, 0, GLenum(format), GetComponentType(), data ? data[1] : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GLenum(m_InternalFormat), width, height, 0, GLenum(format), GetComponentType(), data ? data[3] : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GLenum(m_InternalFormat), width, height, 0, GLenum(format), GetComponentType(), data ? data[2] : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GLenum(m_InternalFormat), width, height, 0, GLenum(format), GetComponentType(), data ? data[5] : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GLenum(m_InternalFormat), width, height, 0, GLenum(format), GetComponentType(), data ? data[4] : nullptr);

		SetMinFilter(GetDefaultFilter());
		SetMagFilter(GetDefaultFilter());
		SetWrapMode(TextureWrap::ClampToEdge);
	}

	TextureFormat GetTextureFormat(TextureComponent component)
	{
		return component == TextureComponent::Color ? TextureFormat::RGBA : TextureFormat::DEPTH;
	}

	InternalTextureFormat GetInternalTextureFormat(TextureComponent component)
	{
		return component == TextureComponent::Color ? InternalTextureFormat::RGBA : InternalTextureFormat::DEPTH;
	}

	RenderTexture::RenderTexture(uint32_t width, uint32_t height, TextureComponent component) : Texture2D(width, height, GetTextureFormat(component), GetInternalTextureFormat(component)),
		m_Framebuffer(Framebuffer::Create(width, height))
	{
		if (component == TextureComponent::Color)
		{
			m_Framebuffer->CreateColorTextureBuffer(ColorBuffer::Color0, this);
			m_Framebuffer->CreateTextureBuffer(ColorBuffer::Depth);
		}
		else
		{
			m_Framebuffer->CreateDepthTextureBuffer(this);
		}
	}

	Ref<RenderTexture> RenderTexture::Create(uint32_t width, uint32_t height, TextureComponent component)
	{
		return CreateRef<RenderTexture>(width, height, component);
	}

}

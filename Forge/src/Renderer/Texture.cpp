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

	Texture2D::Texture2D(uint32_t width, uint32_t height, TextureFormat format, InternalTextureFormat internalFormat) : Texture(GL_TEXTURE_2D, width, height, format, internalFormat)
	{
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, TextureFormat format, InternalTextureFormat internalFormat)
	{
		Ref<Texture2D> texture = CreateRef<Texture2D>(width, height, format, internalFormat);
		texture->Init(nullptr);
		return texture;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& filename)
	{
		int width;
		int height;
		int channels;

		unsigned char* pixels = stbi_load(filename.c_str(), &width, &height, &channels, 4);

		if (pixels)
		{
			Ref<Texture2D> texture = CreateRef<Texture2D>(uint32_t(width), uint32_t(height), TextureFormat::RGBA);
			texture->Init(pixels);
			return texture;
		}
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, const uint8_t* pixels)
	{
		Ref<Texture2D> texture = CreateRef<Texture2D>(width, height, TextureFormat::RGBA, InternalTextureFormat::RGBA);
		texture->Init((const void*)pixels);
		return texture;
	}

	void Texture2D::Init(const void* data)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle.Id);
		Bind();
		glTexImage2D(GL_TEXTURE_2D, 0, GLenum(m_InternalFormat), GetWidth(), GetHeight(), 0, GLenum(m_Format), GetComponentType(), data);
		SetMinFilter(GetDefaultFilter());
		SetMagFilter(GetDefaultFilter());
		SetWrapMode(TextureWrap::Repeat);
	}

	TextureCube::TextureCube(uint32_t width, uint32_t height, TextureFormat format, InternalTextureFormat internalFormat) : Texture(GL_TEXTURE_CUBE_MAP, width, height, format, internalFormat)
	{
	}

	Ref<TextureCube> TextureCube::Create(uint32_t width, uint32_t height, TextureFormat format, InternalTextureFormat internalFormat)
	{
		Ref<TextureCube> texture = CreateRef<TextureCube>(width, height, format, internalFormat);
		texture->Init(nullptr);
		return texture;
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

		Ref<TextureCube> texture = CreateRef<TextureCube>(uint32_t(width), uint32_t(height), TextureFormat::RGBA, InternalTextureFormat::RGBA);
		texture->Init((const void**)pixels);
		for (auto pixel : pixels)
			stbi_image_free(pixel);
		return texture;
	}

	void TextureCube::Init(const void** data)
	{
		uint32_t width = GetWidth();
		uint32_t height = GetHeight();
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_Handle.Id);
		Bind();
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GLenum(m_InternalFormat), width, height, 0, GLenum(m_Format), GetComponentType(), data ? data[0] : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GLenum(m_InternalFormat), width, height, 0, GLenum(m_Format), GetComponentType(), data ? data[1] : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GLenum(m_InternalFormat), width, height, 0, GLenum(m_Format), GetComponentType(), data ? data[3] : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GLenum(m_InternalFormat), width, height, 0, GLenum(m_Format), GetComponentType(), data ? data[2] : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GLenum(m_InternalFormat), width, height, 0, GLenum(m_Format), GetComponentType(), data ? data[5] : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GLenum(m_InternalFormat), width, height, 0, GLenum(m_Format), GetComponentType(), data ? data[4] : nullptr);

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
		m_Framebuffer()
	{
		FramebufferProps props;
		props.Width = width;
		props.Height = height;
		props.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth };
		m_Framebuffer = Framebuffer::Create(props);
		if (component == TextureComponent::Color)
		{
			m_Handle.Id = m_Framebuffer->GetColorAttachment(0)->GetId();
		}
		else
		{
			m_Handle.Id = m_Framebuffer->GetDepthAttachment()->GetId();
		}
		
	}

	RenderTexture::~RenderTexture()
	{
		// Does not own handle
		m_Handle.Id = 0;
	}

	Ref<RenderTexture> RenderTexture::Create(uint32_t width, uint32_t height, TextureComponent component)
	{
		return CreateRef<RenderTexture>(width, height, component);
	}

}

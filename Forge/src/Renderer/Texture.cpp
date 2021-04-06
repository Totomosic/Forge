#include "ForgePch.h"
#include "Texture.h"
#include "Framebuffer.h"

#include <stb_image.h>

namespace Forge
{

	Texture2D::Texture2D(uint32_t width, uint32_t height) : Texture2D(nullptr, width, height, TextureFormat::RGBA)
	{
	}

	Texture2D::Texture2D(const void* pixels, uint32_t width, uint32_t height, TextureFormat format) : Texture(width, height),
		m_Format(format), m_HasMipmaps(false)
	{
		Init(pixels, width, height, uint32_t(format));
	}

	void Texture2D::Bind(int slot) const
	{
		glActiveTexture(GLenum(GL_TEXTURE0 + slot));
		glBindTexture(GL_TEXTURE_2D, GetId());
	}

	void Texture2D::Unbind(int slot) const
	{
		glActiveTexture(GLenum(GL_TEXTURE0 + slot));
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::GenerateMipmaps()
	{
		m_HasMipmaps = true;
		Bind(0);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		return CreateRef<Texture2D>(width, height);
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
		Bind(0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	RenderTexture::RenderTexture(uint32_t width, uint32_t height, TextureComponent component) : Texture2D(width, height),
		m_Framebuffer(Framebuffer::Create(width, height))
	{
		if (component == TextureComponent::Color)
		{
			m_Framebuffer->CreateColorTextureBuffer(ColorBuffer::Color0, this);
			m_Framebuffer->CreateTextureBuffer(ColorBuffer::Depth);
		}
		else
		{
			m_Framebuffer->CreateTextureBuffer(ColorBuffer::Color0);
			m_Framebuffer->CreateDepthTextureBuffer(this);
		}
	}

	Ref<RenderTexture> RenderTexture::Create(uint32_t width, uint32_t height, TextureComponent component)
	{
		return CreateRef<RenderTexture>(width, height, component);
	}

	TextureCube::TextureCube(uint32_t width, uint32_t height) : TextureCube(nullptr, width, height)
	{
	}

	TextureCube::TextureCube(const void** pixels, uint32_t width, uint32_t height, TextureFormat format) : Texture(width, height),
		m_Format(format), m_HasMipmaps(false)
	{
		Init(pixels, width, height, uint32_t(format));
	}

	void TextureCube::Bind(int slot) const
	{
		glActiveTexture(GLenum(GL_TEXTURE0 + slot));
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_Handle.Id);
	}

	void TextureCube::Unbind(int slot) const
	{
		glActiveTexture(GLenum(GL_TEXTURE0 + slot));
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void TextureCube::GenerateMipmaps()
	{
		m_HasMipmaps = true;
		Bind(0);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
		if (data != nullptr)
		{
			Bind(0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, width, height, 0, GLenum(format), GL_UNSIGNED_BYTE, data[0]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, width, height, 0, GLenum(format), GL_UNSIGNED_BYTE, data[1]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, width, height, 0, GLenum(format), GL_UNSIGNED_BYTE, data[3]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, width, height, 0, GLenum(format), GL_UNSIGNED_BYTE, data[2]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, width, height, 0, GLenum(format), GL_UNSIGNED_BYTE, data[5]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, width, height, 0, GLenum(format), GL_UNSIGNED_BYTE, data[4]);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
	}

}

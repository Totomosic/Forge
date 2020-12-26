#include "ForgePch.h"
#include "Texture.h"

#include <stb_image.h>

namespace Forge
{

	Texture2D::Texture2D(uint32_t width, uint32_t height) : Texture2D(nullptr, width, height, TextureFormat::RGBA)
	{
	}

	Texture2D::Texture2D(const void* pixels, uint32_t width, uint32_t height, TextureFormat format) : Texture(width, height),
		m_Format(format)
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

}

#pragma once
#include "ForgePch.h"
#include "Buffer.h"

#include <glad/glad.h>

namespace Forge
{

	namespace Detail
	{

		struct FORGE_API TextureDestructor
		{
		public:
			void operator()(uint32_t id) const
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

	public:
		Texture2D(uint32_t width, uint32_t height);
		Texture2D(const void* pixels, uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA);

		inline TextureFormat GetFormat() const { return m_Format; }

		virtual void Bind(int slot) const override;
		virtual void Unbind(int slot) const override;

	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const std::string& filename);

	private:
		void Init(const void* data, uint32_t width, uint32_t height, uint32_t format);

	};

}

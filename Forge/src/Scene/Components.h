#pragma once
#include "Renderer/Lighting.h"

namespace Forge
{

	struct FORGE_API EnabledFlag
	{
	public:
		// Not used, just here so it is not a void type
		bool Storage;
	};

	struct FORGE_API LayerId
	{
	public:
		uint64_t Mask = 0;

	public:
		LayerId() = default;
		inline LayerId(uint64_t mask)
			: Mask(mask)
		{}
	};

	struct FORGE_API TagComponent
	{
	public:
		std::string Tag;

	public:
		TagComponent(const std::string& name)
			: Tag(name)
		{}
	};

	constexpr uint32_t DEFAULT_SHADOW_WIDTH = 1024;
	constexpr uint32_t DEFAULT_SHADOW_HEIGHT = 1024;

	struct FORGE_API ShadowPass
	{
	public:
		bool Enabled = false;
		Ref<Framebuffer> RenderTarget = nullptr;
		uint64_t LayerMask = 0xFFFFFFFFFFFFFFFF;
	};

	struct FORGE_API LightSourceComponent
	{
	public:
		LightType Type = LightType::Point;
		float Ambient = 0.1f;
		Forge::Color Color = COLOR_WHITE;
		glm::vec3 Attenuation = { 1.0f, 0.0f, 0.0f };
		ShadowPass Shadows;

	public:
		LightSourceComponent(LightType type = LightType::Point, float ambient = 0.1f, const Forge::Color& color = COLOR_WHITE, const glm::vec3& attenuation = { 1.0f, 0.0f, 0.0f })
			: Type(type), Ambient(ambient), Color(color), Attenuation(attenuation)
		{}

		void CreateShadowPass(uint32_t width = DEFAULT_SHADOW_WIDTH, uint32_t height = DEFAULT_SHADOW_HEIGHT)
		{
			Shadows.Enabled = true;
			FramebufferProps props;
			props.Width = width;
			props.Height = height;
			props.Attachments = { { FramebufferTextureFormat::Depth, FramebufferTextureType::TextureCube } };
			Shadows.RenderTarget = Framebuffer::Create(props);
		}
	};

}

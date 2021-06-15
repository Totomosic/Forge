#pragma once
#include "ForgePch.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/CameraData.h"

#include <glm/glm.hpp>

namespace Forge
{

	constexpr uint32_t DEFAULT_SHADOW_WIDTH = 1024;
	constexpr uint32_t DEFAULT_SHADOW_HEIGHT = 1024;

	struct FORGE_API ShadowPass
	{
	public:
		bool Enabled = false;
		Ref<Framebuffer> RenderTarget = nullptr;
		uint64_t LayerMask = 0xFFFFFFFFFFFFFFFF;
	};

	struct FORGE_API CameraComponent
	{
	public:
		Forge::Frustum Frustum;
		Forge::Viewport Viewport;
		uint64_t LayerMask = 0xFFFFFFFFFFFFFFFF;
		int Priority = 0;
		Ref<Framebuffer> RenderTarget = nullptr;
		std::vector<glm::vec4> ClippingPlanes = {};
		Color ClearColor = COLOR_BLACK;
		CameraMode Mode = CameraMode::Normal;
		ShadowPass Shadows;

	public:
		CameraComponent() = default;
		inline CameraComponent(const Forge::Frustum& frustum)
			: Frustum(frustum), Viewport()
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

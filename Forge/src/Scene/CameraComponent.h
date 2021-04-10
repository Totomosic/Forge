#pragma once
#include "ForgePch.h"
#include "Renderer/Framebuffer.h"

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
		glm::mat4 ProjectionMatrix;
		Forge::Viewport Viewport;
		uint64_t LayerMask = 0xFFFFFFFFFFFFFFFF;
		int Priority = 0;
		Ref<Framebuffer> RenderTarget = nullptr;
		std::vector<glm::vec4> ClippingPlanes = {};
		Color ClearColor = COLOR_BLACK;
		ShadowPass Shadows;

	public:
		CameraComponent() = default;
		inline CameraComponent(const glm::mat4& projection)
			: ProjectionMatrix(projection), Viewport()
		{}

		void CreateShadowPass(uint32_t width = DEFAULT_SHADOW_WIDTH, uint32_t height = DEFAULT_SHADOW_HEIGHT)
		{
			Shadows.Enabled = true;
			Shadows.RenderTarget = Framebuffer::Create(width, height);
			Shadows.RenderTarget->CreateTextureBuffer(ColorBuffer::Depth);
		}

	};

}

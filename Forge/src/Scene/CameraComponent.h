#pragma once
#include "ForgePch.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/CameraData.h"

#include <glm/glm.hpp>

namespace Forge
{

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

	public:
		CameraComponent() = default;
		inline CameraComponent(const Forge::Frustum& frustum)
			: Frustum(frustum), Viewport()
		{}
	};

	inline CameraComponent CloneComponent(const CameraComponent& component)
	{
		CameraComponent result = component;
		return result;
	}

}

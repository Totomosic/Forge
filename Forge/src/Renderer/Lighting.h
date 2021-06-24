#pragma once
#include "ForgePch.h"
#include "Core/Color.h"
#include "Framebuffer.h"
#include "CameraData.h"

#include <glm/glm.hpp>

namespace Forge
{

	constexpr int MAX_LIGHT_COUNT = 8;

	enum class LightType : uint8_t
	{
		Point,
		Directional,
		Spotlight,
	};

	struct FORGE_API LightSource
	{
	public:
		LightType Type = LightType::Point;
		glm::vec3 Position = { 0, 0, 0 };
		float Ambient = 0.2f;
		Forge::Color Color = COLOR_WHITE;
		glm::vec3 Attenuation = { 1, 0, 0 };
		glm::vec3 Direction = { 0, 0, 0 };
		float Intensity = 1.0f;

		// Shadows
		Ref<Framebuffer> ShadowFramebuffer = nullptr;
		Frustum ShadowFrustum;
		mutable int ShadowBindLocation;
		mutable glm::mat4 LightSpaceTransform;
	};

}

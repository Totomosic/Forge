#pragma once
#include "ForgePch.h"
#include "Core/Color.h"

#include <glm/glm.hpp>

namespace Forge
{

	constexpr int MAX_LIGHT_COUNT = 32;

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
		float Ambient = 0.1f;
		Forge::Color Color = COLOR_WHITE;
		glm::vec3 Attenuation = { 1, 0, 0 };
		glm::vec3 Direction = { 0, 0, 0 };


	};

}

#pragma once
#include "Renderer/Lighting.h"

namespace Forge
{

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

	struct FORGE_API LightSourceComponent
	{
	public:
		LightType Type = LightType::Point;
		float Ambient = 0.1f;
		Forge::Color Color = COLOR_WHITE;
		glm::vec3 Attenuation = { 1.0f, 0.0f, 0.0f };

	public:
		LightSourceComponent(LightType type = LightType::Point, float ambient = 0.1f, const Forge::Color& color = COLOR_WHITE, const glm::vec3& attenuation = { 1.0f, 0.0f, 0.0f })
			: Type(type), Ambient(ambient), Color(color), Attenuation(attenuation)
		{}
	};

}

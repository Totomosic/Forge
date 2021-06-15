#pragma once
#include "Logging.h"

namespace Forge
{

	struct FORGE_API Color
	{
	public:
		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
		float a = 1.0f;

	public:
		Color() = default;
		inline constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
			: r(r / 255.0f), g(g / 255.0f), b(b / 255.0f), a(a / 255.0f)
		{}

		inline Color WithAlpha(uint8_t alpha) const
		{
			Color result = *this;
			result.a = alpha / 255.0f;
			return result;
		}
	};

	constexpr Color COLOR_RED = { 255, 0, 0, 255 };
	constexpr Color COLOR_GREEN = { 0, 255, 0, 255 };
	constexpr Color COLOR_BLUE = { 0, 0, 255, 255 };
	constexpr Color COLOR_BLACK = { 0, 0, 0, 255 };
	constexpr Color COLOR_WHITE = { 255, 255, 255, 255 };

	constexpr Color SKY_BLUE = { 135, 206, 235, 255 };

}

#pragma once
#include "Logging.h"

namespace Forge
{

	struct FORGE_API Color
	{
	public:
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;
		uint8_t a = 255;

	public:
		inline Color WithAlpha(uint8_t alpha) const { return Color{ r, g, b, alpha }; }
	};

	constexpr Color COLOR_RED = { 255, 0, 0, 255 };
	constexpr Color COLOR_GREEN = { 0, 255, 0, 255 };
	constexpr Color COLOR_BLUE = { 0, 0, 255, 255 };
	constexpr Color COLOR_BLACK = { 0, 0, 0, 255 };
	constexpr Color COLOR_WHITE = { 255, 255, 255, 255 };

	constexpr Color SKY_BLUE = { 135, 206, 235, 255 };

}

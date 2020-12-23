#pragma once
#include "ForgePch.h"

namespace Forge
{

	struct FORGE_API WindowResize
	{
	public:
		uint32_t NewWidth;
		uint32_t NewHeight;
		uint32_t OldWidth;
		uint32_t OldHeight;
	};

	struct FORGE_API WindowClose
	{
	};

}

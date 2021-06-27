#pragma once
#include "Logging.h"

#include <string>

namespace Forge
{

	class FORGE_API FileUtils
	{
	public:
		static bool Exists(const std::string& filepath);
		static std::string ReadTextFile(const std::string& filepath);
	};

	class FORGE_API FileDialogs
	{
	public:
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};

}

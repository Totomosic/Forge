#pragma once
#include "ForgePch.h"

namespace Forge
{
	
	constexpr int MAX_CLIPPING_PLANES = 8;

	class FORGE_API ShaderLibrary
	{
	private:
		static std::unordered_map<std::string, std::string> s_ShaderSources;

	public:
		static bool HasShaderSource(const std::string& filename);
		static const std::string& GetShaderSource(const std::string& filename);
	};

}

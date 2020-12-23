#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>

#define FORGE_API

namespace Forge
{

	class FORGE_API Logger
	{
	private:
		static bool s_Initialized;
		static std::shared_ptr<spdlog::logger> s_Logger;

	public:
		static void Init();
		static inline spdlog::logger& GetLogger() { return *s_Logger; }
	};

}

#ifdef FORGE_DIST 
#define FORGE_TRACE(...)
#define FORGE_INFO(...)
#define FORGE_WARN(...)
#define FORGE_ERROR(...)
#define FORGE_FATAL(...)

#define FORGE_ASSERT(arg, ...)

#define FORGE_DEBUG_ONLY(x)
#else

#define FORGE_TRACE(...) ::Forge::Logger::GetLogger().trace(__VA_ARGS__)
#define FORGE_INFO(...) ::Forge::Logger::GetLogger().info(__VA_ARGS__)
#define FORGE_WARN(...) ::Forge::Logger::GetLogger().warn(__VA_ARGS__)
#define FORGE_ERROR(...) ::Forge::Logger::GetLogger().error(__VA_ARGS__)
#define FORGE_FATAL(...) ::Forge::Logger::GetLogger().critical(__VA_ARGS__)

#ifdef FORGE_PLATFORM_WINDOWS
#define FORGE_ASSERT(arg, ...) { if (!(arg)) { FORGE_FATAL(__VA_ARGS__); __debugbreak(); } }
#else
#define FORGE_ASSERT(arg, ...) { if (!(arg)) { FORGE_FATAL(__VA_ARGS__); } }
#endif

#define FORGE_DEBUG_ONLY(x) x
#endif

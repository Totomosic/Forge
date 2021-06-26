project "ForgeEditor"
    location ""
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    targetdir ("../bin/" .. OutputTemplate .. "/%{prj.name}")
    objdir ("../bin-int/" .. OutputTemplate .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.cpp"
    }
    
    includedirs
    {
        "../%{IncludeDirs.GLFW}",
        "../%{IncludeDirs.Glad}",
		"../%{IncludeDirs.ImGui}",
        "../%{IncludeDirs.spdlog}",
        "../%{IncludeDirs.glm}",
        "../%{IncludeDirs.entt}",
        "../%{IncludeDirs.tinygltf}",
        "../%{IncludeDirs.Forge}",
        "../%{IncludeDirs.ImGuizmo}",
        "../%{IncludeDirs.yaml_cpp}",
    }

    links
    {
        "Forge",
        "opengl32.lib",
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "FORGE_PLATFORM_WINDOWS",
            "FORGE_BUILD_STATIC",
            "_CRT_SECURE_NO_WARNINGS",
            "NOMINMAX",
            "GLEW_STATIC"
        }

    filter "system:linux"
        systemversion "latest"

        defines
        {
            "FORGE_PLATFORM_LINUX",
            "FORGE_BUILD_STATIC",
            "GLEW_STATIC"
        }

        links 
        {
            "stdc++fs"
        }

    filter "system:macosx"
        systemversion "latest"

        defines
        {
            "FORGE_PLATFORM_MAC",
            "FORGE_BUILD_STATIC",
            "GLEW_STATIC"
        }

        links 
        {
            "stdc++fs"
        }

    filter "configurations:Debug"
        defines "FORGE_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "FORGE_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "FORGE_DIST"
        runtime "Release"
        optimize "on"
workspace "Forge"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    flags
    {
        "MultiProcessorCompile"
    }

include ("Paths.lua")

group ("Forge/Vendor")
include (ForgeCoreDir .. "vendor/glfw")
include (ForgeCoreDir .. "vendor/Glad")
include (ForgeCoreDir .. "vendor/imgui")
include (ForgeCoreDir .. "vendor/glm")
group ("Forge")
include (ForgeCoreDir)
group("Editor")
include ("Editor")
group ("Sandbox")
include ("Sandbox")
include ("MarchingCubes")

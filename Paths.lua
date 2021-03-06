OutputTemplate = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Forge Directory
ForgeCoreDir = "Forge/"

-- Include directories relative to solution directory
IncludeDirs = {}
IncludeDirs["Forge"] =          ForgeCoreDir .. "src"
IncludeDirs["GLFW"] =           ForgeCoreDir .. "vendor/GLFW/include"
IncludeDirs["Glad"] =           ForgeCoreDir .. "vendor/Glad/include"
IncludeDirs["ImGui"] =          ForgeCoreDir .. "vendor/ImGui"
IncludeDirs["spdlog"] =         ForgeCoreDir .. "vendor/spdlog/include"
IncludeDirs["glm"] =            ForgeCoreDir .. "vendor/glm"
IncludeDirs["entt"] =           ForgeCoreDir .. "vendor/entt/src"
IncludeDirs["tinygltf"] =       ForgeCoreDir .. "vendor/tinygltf"
IncludeDirs["ImGuizmo"] =       ForgeCoreDir .. "vendor/ImGuizmo"
IncludeDirs["yaml_cpp"] =       ForgeCoreDir .. "vendor/yaml-cpp/include"

-- Library directories relative to solution directory
LibraryDirs = {}

-- Links
Links = {}
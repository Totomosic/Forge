#include "ForgePch.h"
#include "ShaderLibrary.h"

#include "Renderer/Lighting.h"

namespace Forge
{

    std::unordered_map<std::string, std::string> ShaderLibrary::s_ShaderSources = {
        { 
            "Lighting.h",

            "const int LIGHT_TYPE_POINT = 0;\n"
            "const int LIGHT_TYPE_DIRECTIONAL = 1;\n"
            "const int LIGHT_TYPE_SPOTLIGHT = 2;\n"
            "const int MAX_LIGHT_COUNT = " + std::to_string(MAX_LIGHT_COUNT) + ";\n"
            "struct LightSource\n"
            "{\n"
            "int Type;\n"
            "vec3 Position;\n"
            "vec3 Direction;\n"
            "float Ambient;\n"
            "vec4 Color;\n"
            "vec3 Attenuation;\n"
            "};\n"
        }
    };

    bool ShaderLibrary::HasShaderSource(const std::string& filename)
    {
        return s_ShaderSources.find(filename) != s_ShaderSources.end();
    }

    const std::string& ShaderLibrary::GetShaderSource(const std::string& filename)
    {
        FORGE_ASSERT(HasShaderSource(filename), "Filename {} does not exist", filename);
        return s_ShaderSources[filename];
    }

}

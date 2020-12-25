#include "ForgePch.h"
#include "ShaderLibrary.h"

namespace Forge
{

    std::unordered_map<std::string, std::string> s_ShaderSources = {
        { 
            "Lighting.h",

            "struct LightSource\n"
            "{\n"
            "int Type;\n"
            "vec3 Position;\n"
            "vec3 Direction;\n"
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
#include "ForgePch.h"
#include "ShaderLibrary.h"

#include "Renderer/Lighting.h"

namespace Forge
{

    std::unordered_map<std::string, std::string> ShaderLibrary::s_ShaderSources = {
        {
            "Constants.h",

#include "Shaders/Constants.h"
        },
        { 
            "LightingUtils.h",

            "const int MAX_LIGHT_COUNT = " + std::to_string(MAX_LIGHT_COUNT) + ";\n"
#include "Shaders/LightingUtils.h"
        },
        {
            "Lighting.h",

#include "Shaders/Lighting.h"
        },
        {
            "Shadows.h",
            
#include "Shaders/Shadows.h"
        },
        {
            "Clipping.h",

            "const int MAX_CLIPPING_PLANES = " + std::to_string(MAX_CLIPPING_PLANES) + ";\n"
#include "Shaders/Clipping.h"
        },
        {
            "PBRUtils.h",

#include "Shaders/PBRUtils.h"
        },
        {
            "PBR.h",

#include "Shaders/PBR.h"
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

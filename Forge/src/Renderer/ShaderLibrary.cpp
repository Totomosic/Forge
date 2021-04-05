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
            "\n"
            "vec4 calculateLightDiffuse(vec3 position, vec3 normal, LightSource lightSources[MAX_LIGHT_COUNT], int usedLights)\n"
            "{\n"
            "    vec3 color = vec3(0.0);\n"
            "    for (int i = 0; i < usedLights; i++)\n"
            "    {\n"
            "        vec3 lightDirection = normalize(lightSources[i].Position - position);\n"
            "        float diffusePower = max(dot(normal, lightDirection), 0.0);\n"
            "        vec4 diffuseColor = diffusePower * lightSources[i].Color;\n"
            "        color += diffuseColor.xyz + lightSources[i].Ambient * lightSources[i].Color.xyz;\n"
            "    }\n"
            "    return vec4(color, 1.0);\n"
            "}\n"
        },
        {
            "Clipping.h",

            "const int MAX_CLIPPING_PLANES = " + std::to_string(MAX_CLIPPING_PLANES) + ";\n"
            "float clipPlane(vec3 worldPosition, vec4 plane)\n"
            "{\n"
            "   return dot(vec4(worldPosition, 1.0), plane);\n"
            "}\n\n"
            "void clipPlanes(vec3 worldPosition, vec4 plane[MAX_CLIPPING_PLANES], int usedClippingPlanes)\n"
            "{\n"
            "   for (int i = 0; i < usedClippingPlanes; i++)\n"
            "   {\n"
            "       gl_ClipDistance[i] = clipPlane(worldPosition, plane[i]);\n"
            "   }\n"
            "}\n"
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

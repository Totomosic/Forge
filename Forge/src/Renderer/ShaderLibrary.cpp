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
            "vec4 calculateLightDiffuse(vec3 position, vec3 normal, LightSource lightSources[MAX_LIGHT_COUNT], int usedLights, float shadow)\n"
            "{\n"
            "    vec3 color = vec3(0.0);\n"
            "    for (int i = 0; i < usedLights; i++)\n"
            "    {\n"
            "        vec3 lightDirection = normalize(lightSources[i].Position - position);\n"
            "        float diffusePower = max(dot(normal, lightDirection), 0.0);\n"
            "        vec4 diffuseColor = diffusePower * lightSources[i].Color * (1.0 - shadow);\n"
            "        color += diffuseColor.xyz + lightSources[i].Ambient * lightSources[i].Color.xyz;\n"
            "    }\n"
            "    return vec4(color, 1.0);\n"
            "}\n\n"
            "vec4 calculateLightSpecular(vec3 position, vec3 normal, float specularIntensity, float damping, vec3 toCamera, LightSource lightSources[MAX_LIGHT_COUNT], int usedLights, float shadow)\n"
            "{\n"
            "    vec3 unitToCamera = normalize(toCamera);\n"
            "    vec3 color = vec3(0.0);\n"
            "    for (int i = 0; i < usedLights; i++)\n"
            "    {\n"
            "        vec3 toLightVector = normalize(lightSources[i].Position - position);\n"
            "        vec3 lightDirection = -toLightVector;\n"
            "        vec3 reflectedLightDirection = reflect(lightDirection, normal);\n"
            "        float specularFactor = max(dot(reflectedLightDirection, unitToCamera), 0.0);\n"
            "        float dampedSpecularFactor = pow(specularFactor, damping);\n"
            "        color += specularIntensity * dampedSpecularFactor * lightSources[i].Color.xyz * (1.0 - shadow);\n"
            "    }\n"
            "    return vec4(color, 1.0);\n"
            "}\n\n"
            "vec4 calculateLightDiffuse(vec3 position, vec3 normal, LightSource lightSources[MAX_LIGHT_COUNT], int usedLights)\n"
            "{\n"
            "    return calculateLightDiffuse(position, normal, lightSources, usedLights, 0.0);\n"
            "}\n\n"
            "vec4 calculateLightSpecular(vec3 position, vec3 normal, float specularIntensity, float damping, vec3 toCamera, LightSource lightSources[MAX_LIGHT_COUNT], int usedLights)\n"
            "{\n"
            "    return calculateLightSpecular(position, normal, specularIntensity, damping, toCamera, lightSources, usedLights, 0.0);\n"
            "}\n"
        },
        {
            "Shadows.h",
            
            "float calculateShadow(vec4 positionLightSpace, sampler2D shadowMap, vec3 normal, vec3 lightDirection)\n"
            "{\n"
            "   float bias = max(0.005 * (1.0 - abs(dot(normal, lightDirection))), 0.0005);\n"
            "   vec2 texelSize = 1.0 / textureSize(shadowMap, 0);\n"
            "   float shadow = 0.0;\n"
            "   vec3 projCoords = positionLightSpace.xyz / positionLightSpace.w;\n"
            "   projCoords = projCoords * 0.5 + 0.5;\n"
            "   for (int x = -1; x <= 1; x++)\n"
            "   {\n"
            "       for (int y = -1; y <= 1; y++)\n"
            "       {\n"
            "           float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;\n"
            "           shadow += projCoords.z - bias > pcfDepth ? 1.0 : 0.0;\n"
            "       }\n"
            "   }\n"
            "   float closestDepth = texture(shadowMap, projCoords.xy).r;\n"
            "   return shadow / 9.0;\n"
            "}\n"
            "\n"
            "float calculatePointShadow(vec3 position, samplerCube shadowMap, float farPlane, vec3 lightPosition)\n"
            "{\n"
            "   float bias = 0.1;\n"
            "   vec3 lightToFrag = position - lightPosition;\n"
            "   float closestDepth = texture(shadowMap, lightToFrag).r;\n"
            "   closestDepth *= farPlane;\n"
            "   float currentDepth = length(lightToFrag);\n"
            "   return currentDepth - bias > closestDepth ? 1.0 : 0.0;\n"
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

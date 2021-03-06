#include <PBRUtils.h>
#include <Shadows.h>

layout(std140, binding = 3) uniform LightSources
{
    LightSource frg_LightSources[MAX_LIGHT_COUNT];
    int frg_UsedLightSources;
};

#ifdef SHADOW_MAP
uniform LightSourceShadowMaps frg_LightShadowMaps[MAX_LIGHT_COUNT];
#endif

["Emission"]
uniform float u_Emission;

vec4 CalculateLightingPBR(vec3 position, vec3 normal, vec3 cameraPosition, PBRMaterialOptions material)
{
    vec3 toCameraVector = cameraPosition - position;
    vec3 unitToCameraVector = normalize(toCameraVector);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, material.Albedo.xyz, material.Metallic);

    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    for (int i = 0; i < frg_UsedLightSources; i++)
    {
        float shadow = 0.0;
#ifdef SHADOW_MAP
        if (frg_LightSources[i].UseShadows)
        {
            if (frg_LightSources[i].Type == LIGHT_TYPE_POINT)
                shadow = CalculatePointShadow(position, frg_LightShadowMaps[i].PointShadowMap, frg_LightSources[i].ShadowFar, frg_LightSources[i].Position, cameraPosition);
            else
            {
                vec4 lightSpacePosition = frg_LightSources[i].LightSpaceTransform * vec4(position, 1.0);
                shadow = CalculateShadow(lightSpacePosition, frg_LightShadowMaps[i].ShadowMap, normal, frg_LightSources[i].Direction);
            }
        }
#endif
        vec4 lighting = CalculateSinglePBRLight(position, normal, F0, unitToCameraVector, frg_LightSources[i], material, shadow);
        color.xyz += lighting.xyz;
        color.a = max(color.a, lighting.a);
    }

    color.xyz += material.Albedo.xyz * u_Emission;
    return color;
}

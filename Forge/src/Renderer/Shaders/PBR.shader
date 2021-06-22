#include <PBRUtils.h>
#include <Shadows.h>

uniform LightSource frg_LightSources[MAX_LIGHT_COUNT];
uniform int frg_UsedLightSources;
#ifdef SHADOW_MAP
uniform mat4 frg_LightSpaceTransform;
#endif

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
                shadow = CalculatePointShadow(position, frg_LightSources[i].PointShadowMap, frg_LightSources[i].ShadowFar, frg_LightSources[i].Position, cameraPosition);
            else
            {
                vec4 lightSpacePosition = frg_LightSpaceTransform * vec4(position, 1.0);
                // shadow = CalculateShadow(lightSpacePosition, frg_LightSources[i].ShadowMap, normal, frg_LightSources[i].Direction);
            }
        }
#endif
        vec4 lighting = CalculateSinglePBRLight(position, normal, F0, unitToCameraVector, frg_LightSources[i], material, shadow);
        color.xyz += lighting.xyz;
        color.a = max(color.a, lighting.a);
    }

    color.xyz = color.xyz / (color.xyz + vec3(1.0));
    color.xyz = pow(color.xyz, vec3(1.0 / 2.2));

    return color;
}

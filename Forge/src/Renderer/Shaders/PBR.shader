#include <PBRUtils.h>
#include <Shadows.h>

uniform LightSource frg_LightSources[MAX_LIGHT_COUNT];
uniform int frg_UsedLightSources;
uniform float frg_FarPlane;

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
            shadow = CalculatePointShadow(position, frg_LightSources[i].ShadowMap, frg_FarPlane, frg_LightSources[i].Position, cameraPosition);
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

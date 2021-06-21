#include <LightingUtils.h>
#include <Shadows.h>

struct MaterialOptions
{
    float Diffuse;
    float Specular;
    float ShineDamper;
};

uniform LightSource frg_LightSources[MAX_LIGHT_COUNT];
uniform int frg_UsedLightSources;
uniform float frg_FarPlane;

vec4 CalculateLighting(vec3 position, vec3 normal, vec3 cameraPosition, MaterialOptions material)
{
    vec3 toCameraVector = cameraPosition - position;
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
        vec4 diffuse = CalculateSingleLightDiffuse(position, normal, frg_LightSources[i], shadow);
        vec4 specular = CalculateSingleLightSpecular(position, normal, material.Specular, material.ShineDamper, toCameraVector, frg_LightSources[i], shadow);
        color.xyz += diffuse.xyz + specular.xyz;
        color.a = max(color.a, diffuse.a);
    }
    return color;
}

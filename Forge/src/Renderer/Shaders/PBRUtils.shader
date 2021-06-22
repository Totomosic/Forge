#include <Constants.h>
#include <LightingUtils.h>

struct PBRMaterialOptions
{
    vec4 Albedo;
    float Metallic;
    float Roughness;
    float AmbientOcclusion;
};

// F0 - surface reflection at zero incidence angle (how much the surface reflects when directly facing the light source)
// Most dielectric surfaces look good with F0 = 0.04
vec3 FresnelSchlick(float cosTheta, vec3 f0)
{
    return f0 + (1.0 - f0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float nDotH = max(dot(N, H), 0.0);
    float nDotH2 = nDotH * nDotH;

    float numerator = a2;
    float denom = (nDotH2 * (a2 - 1.0) + 1.0);
    float newDenom = PI * denom * denom;
    return numerator / newDenom;
}

float GeometrySchlickGGX(float nDotV, float roughness)
{
    float r = roughness + 1.0;
    float k = r * r / 8.0;

    float numerator = nDotV;
    float denom = nDotV * (1.0 - k) + k;

    return numerator / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float nDotV = max(dot(N, V), 0.0);
    float nDotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(nDotV, roughness);
    float ggx1 = GeometrySchlickGGX(nDotL, roughness);
    return ggx1 * ggx2;
}

vec4 CalculateSinglePBRLight(vec3 position, vec3 normal, vec3 F0, vec3 unitToCameraVector, LightSource light, PBRMaterialOptions material, float shadow)
{
    vec3 N = normal;
    vec3 V = unitToCameraVector;
    vec3 toLightVector;
    if (light.Type == LIGHT_TYPE_POINT)
        toLightVector = light.Position - position;
    else if (light.Type == LIGHT_TYPE_DIRECTIONAL)
        toLightVector = -light.Direction;
    float distance = length(toLightVector);
    vec3 L = toLightVector / distance;
    vec3 H = normalize(V + L);
    float attenuation = 1.0;
    if (light.Type == LIGHT_TYPE_POINT)
        attenuation = 1.0 / (light.Attenuation.x + distance * light.Attenuation.y + distance * distance * light.Attenuation.z);

    vec4 radiance = vec4(light.Color.xyz * light.Intensity * attenuation, light.Color.a);
    float ndf = DistributionGGX(N, H, material.Roughness);
    float g = GeometrySmith(N, V, L, material.Roughness);
    vec3 f = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 kS = f;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - material.Metallic;

    vec3 numerator = ndf * f * g;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denominator, 0.001);

    float nDotL = max(dot(N, L), 0.0);
    return vec4( (kD * material.Albedo.xyz / PI + specular) * radiance.xyz * nDotL * (1.0 - shadow) + material.Albedo.xyz * light.Ambient * material.AmbientOcclusion, radiance.a * material.Albedo.a );
}


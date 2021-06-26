const int LIGHT_TYPE_POINT = 0;
const int LIGHT_TYPE_DIRECTIONAL = 1;
const int LIGHT_TYPE_SPOTLIGHT = 2;

struct LightSource
{
    int Type;
    vec3 Position;
    vec3 Direction;
    float Ambient;
    vec4 Color;
    vec3 Attenuation;
    float Intensity;
    bool UseShadows;
    samplerCube PointShadowMap;
    sampler2D ShadowMap;
    float ShadowNear;
    float ShadowFar;
    mat4 LightSpaceTransform;
};

vec4 CalculateSingleLightDiffuse(vec3 position, vec3 normal, LightSource light, float shadow)
{
    vec3 toLightVector;
    if (light.Type == LIGHT_TYPE_POINT)
        toLightVector = light.Position - position;
    else if (light.Type == LIGHT_TYPE_DIRECTIONAL)
        toLightVector = -light.Direction;
    float distance = length(toLightVector);
    vec3 lightDirection = toLightVector / distance;
    float diffusePower = max(dot(normal, lightDirection), 0.0);
    float attenuation = 1.0;
    if (light.Type == LIGHT_TYPE_POINT)
        attenuation = 1.0 / (light.Attenuation.x + distance * light.Attenuation.y + distance * distance * light.Attenuation.z);
    vec3 color = diffusePower * light.Color.xyz * light.Intensity * (1.0 - shadow) + light.Color.xyz * light.Ambient * attenuation;
    return vec4(color * attenuation, light.Color.a);
}

vec4 CalculateSingleLightSpecular(vec3 position, vec3 normal, float specularIntensity, float shineDamping, vec3 toCamera, LightSource light, float shadow)
{
    vec3 unitToCamera = normalize(toCamera);
    vec3 toLightVector;
    if (light.Type == LIGHT_TYPE_POINT)
        toLightVector = light.Position - position;
    else if (light.Type == LIGHT_TYPE_DIRECTIONAL)
        toLightVector = -light.Direction;
    float distance = length(toLightVector);
    vec3 unitToLightVector = toLightVector / distance;
    vec3 lightDirection = -unitToLightVector;
    vec3 reflectedLightDirection = reflect(lightDirection, normal);
    float specularFactor = max(dot(reflectedLightDirection, unitToCamera), 0.0);
    float dampedSpecularFactor = pow(specularFactor, shineDamping);
    float attenuation = 1.0;
    if (light.Type == LIGHT_TYPE_POINT)
        attenuation = 1.0 / (light.Attenuation.x + distance * light.Attenuation.y + distance * distance * light.Attenuation.z);
    vec3 color = specularIntensity * dampedSpecularFactor * light.Intensity * light.Color.xyz * (1.0 - shadow);
    return vec4(color * attenuation, light.Color.a);
}

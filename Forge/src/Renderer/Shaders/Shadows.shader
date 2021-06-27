struct LightSourceShadowMaps
{
    sampler2D ShadowMap;
    samplerCube PointShadowMap;
};

float CalculateShadow(vec4 positionLightSpace, sampler2D shadowMap, vec3 normal, vec3 lightDirection)
{
   float bias = 0.0; // max(0.005 * (1.0 - abs(dot(normal, lightDirection))), 0.0005);
   vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
   float shadow = 0.0;
   vec3 projCoords = positionLightSpace.xyz / positionLightSpace.w;
   projCoords = projCoords * 0.5 + 0.5;
   for (int x = -1; x <= 1; x++)
   {
       for (int y = -1; y <= 1; y++)
       {
           float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
           shadow += projCoords.z - bias > pcfDepth ? 1.0 : 0.0;
       }
   }
   float closestDepth = texture(shadowMap, projCoords.xy).r;
   return shadow / 9.0;
}

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float CalculatePointShadow(vec3 position, samplerCube shadowMap, float farPlane, vec3 lightPosition, vec3 cameraPosition)
{
   float bias = 0.00;
   float viewDistance = length(cameraPosition - position);
   float diskRadius = (0.1 + (4.0 * viewDistance / farPlane)) / 25.0;
   vec3 lightToFrag = position - lightPosition;
   float currentDepth = length(lightToFrag);
   float shadow = 0.0;
   for (int i = 0; i < 20; i++)
   {
       float closestDepth = texture(shadowMap, lightToFrag + sampleOffsetDirections[i] * diskRadius).r;
       closestDepth *= farPlane;
       if (currentDepth - bias > closestDepth)
           shadow += 1.0;
   }
   return shadow / 20.0;
}

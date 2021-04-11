#shader VERTEX
#version 430 core
#include "Clipping.h"

in layout(location = 0) vec3 in_Position;
in layout(location = 1) vec3 in_Normal;
in layout(location = 2) vec2 in_TexCoord;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjViewMatrix;
uniform vec4 u_ClippingPlanes[MAX_CLIPPING_PLANES];
uniform int u_UsedClippingPlanes;
uniform mat4 u_LightSpaceTransform;

out vec3 f_Position;
out vec3 f_Normal;
out vec2 f_TexCoord;
out vec4 f_ClipSpace;
out vec3 f_ToCameraVector;
out vec4 f_LightSpacePosition;

void main()
{
	vec4 worldPosition = u_ModelMatrix * vec4(in_Position, 1.0);
	clipPlanes(worldPosition.xyz, u_ClippingPlanes, u_UsedClippingPlanes);
	gl_Position = u_ProjViewMatrix * worldPosition;
	f_Normal = (transpose(inverse(u_ModelMatrix)) * vec4(in_Normal, 0.0)).xyz;
	f_Position = worldPosition.xyz;
	f_ClipSpace = gl_Position;

	vec3 cameraPosition = (inverse(u_ViewMatrix)[3]).xyz;
	f_ToCameraVector = cameraPosition - f_Position;
	f_TexCoord = in_TexCoord * 80.0;
	f_LightSpacePosition = u_LightSpaceTransform * worldPosition;
}

#shader FRAGMENT
#version 430 core
#include "Lighting.h"
#include "Shadows.h"

out layout(location = 0) vec4 out_FragColor;

uniform vec4 u_Color;
uniform LightSource u_LightSources[MAX_LIGHT_COUNT];
uniform int u_UsedLightSources;
uniform samplerCube u_ShadowMap;
uniform vec3 u_LightPosition;

uniform sampler2D u_RefractionTexture;
uniform sampler2D u_ReflectionTexture;
uniform sampler2D u_DepthTexture;
uniform sampler2D u_NormalMap;
uniform sampler2D u_DUDVMap;
uniform float u_FarPlane;
uniform float u_NearPlane;
uniform float u_Time;

const float waveStrength = 0.05;

in vec3 f_Position;
in vec3 f_Normal;
in vec2 f_TexCoord;
in vec4 f_ClipSpace;
in vec3 f_ToCameraVector;
in vec4 f_LightSpacePosition;

// const vec4 BLUE = vec4(0.0, 58.0 / 255.0, 78.0 / 255.0, 1.0);
const vec4 BLUE = vec4(0.1, 0.5, 0.8, 1.0);

float unprojectDepth(float depth)
{
	return 2.0 * u_NearPlane * u_FarPlane / (u_FarPlane + u_NearPlane - (2.0 * depth - 1.0) * (u_FarPlane - u_NearPlane));
}

void main()
{
	vec3 unitNormal = normalize(f_Normal);
	vec3 unitToCamera = normalize(f_ToCameraVector);
	vec2 textureCoords = f_ClipSpace.xy / f_ClipSpace.w / 2.0 + 0.5;
	vec2 refractCoords = vec2(textureCoords.x, textureCoords.y);
	vec2 reflectCoords = vec2(textureCoords.x, 1.0 - textureCoords.y);

	float depth = texture(u_DepthTexture, refractCoords).r;
	float floorDistance = unprojectDepth(depth);
	float waterDistance = unprojectDepth(gl_FragCoord.z);
	float waterDepth = floorDistance - waterDistance;

	vec2 distortedTextureCoords = texture(u_DUDVMap, vec2(f_TexCoord.x + u_Time, f_TexCoord.y)).rg * 0.1;
	distortedTextureCoords = f_TexCoord + vec2(distortedTextureCoords.x, distortedTextureCoords.y + u_Time);
	vec2 totalDistortion = (texture(u_DUDVMap, distortedTextureCoords).rg * 2.0 - 1.0) * waveStrength * clamp(waterDepth / 5.0, 0.0, 1.0);

	vec4 normalMapValue = texture(u_NormalMap, distortedTextureCoords);
	vec3 worldNormal = vec3(normalMapValue.r * 2.0 - 1.0, normalMapValue.b * 3.0, normalMapValue.g * 2.0 - 1.0);
	unitNormal = normalize(worldNormal);

	refractCoords += totalDistortion;
	refractCoords = clamp(refractCoords, 0.001, 0.999);
	reflectCoords += totalDistortion;
	reflectCoords = clamp(reflectCoords, 0.001, 0.999);

	vec4 refraction = texture(u_RefractionTexture, refractCoords);
	vec4 reflection = texture(u_ReflectionTexture, reflectCoords);

	float refractiveFactor = pow(dot(unitToCamera, unitNormal), 0.5);

	vec4 color = mix(reflection, refraction, max(refractiveFactor, 0.0));

	float shadow = calculatePointShadow(f_Position, u_ShadowMap, u_FarPlane, u_LightPosition);

	out_FragColor = BLUE * color * (calculateLightDiffuse(f_Position, unitNormal, u_LightSources, u_UsedLightSources, shadow) + calculateLightSpecular(f_Position, unitNormal, 8.0, 10.0, unitToCamera, u_LightSources, u_UsedLightSources, shadow));
	out_FragColor.a = clamp(waterDepth / 0.5, 0.0, 1.0);
}

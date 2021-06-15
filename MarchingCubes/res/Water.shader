#shader VERTEX
#version 430 core
#include "Clipping.h"

in layout(location = 0) vec3 in_Position;
in layout(location = 1) vec3 in_Normal;
in layout(location = 2) vec2 in_TexCoord;

uniform mat4 frg_ModelMatrix;
uniform mat4 frg_ViewMatrix;
uniform mat4 frg_ProjViewMatrix;
uniform vec4 frg_ClippingPlanes[MAX_CLIPPING_PLANES];
uniform int frg_UsedClippingPlanes;
uniform mat4 frg_LightSpaceTransform;
uniform float u_Time;

out vec3 g_Position;
out vec2 g_TexCoord;
out vec4 g_ClipSpace;
out vec3 g_ToCameraVector;
out vec4 g_LightSpacePosition;

out VData
{
	vec3 Position;
	vec2 TexCoord;
	vec4 ClipSpace;
	vec3 ToCameraVector;
	vec4 LightSpacePosition;
} OutData;

const float PI = 3.14159;
const float GRAVITY = 20.0;
const float WAVE_SCALE = 0.25;

float wave(vec4 position, float x, float z, float time, float scale)
{
	return scale * sin(position.x * x + u_Time * time) * sin(position.z * z + u_Time * time);
}

vec3 gerstnerWave(vec3 position, float time, float steepness, float wavelength, vec2 direction)
{
	float k = 2 * PI / wavelength;
	float c = sqrt(GRAVITY / k);
	vec2 d = normalize(direction);
	float f = k * (dot(position.xz, d) - c * time);
	float a = WAVE_SCALE * steepness / k;
	
	return vec3(d.x * (a * cos(f)), a * sin(f), d.y * (a * cos(f)));
}

void main()
{
	vec4 worldPosition = frg_ModelMatrix * vec4(in_Position, 1.0);

	//float height = wave(worldPosition, 2.5, 2.5, 8.0, 0.05);
	//worldPosition.y += height;
	vec3 position = worldPosition.xyz;
	worldPosition.xyz += gerstnerWave(position, u_Time, 0.25, 3.00, vec2( 1.0, 1.0 ));
	worldPosition.xyz += gerstnerWave(position, u_Time, 0.25, 1.52, vec2( 1.0, 0.6 ));
	worldPosition.xyz += gerstnerWave(position, u_Time, 0.25, 0.90, vec2( 1.0, 1.3 ));

	clipPlanes(worldPosition.xyz, frg_ClippingPlanes, frg_UsedClippingPlanes);
	gl_Position = frg_ProjViewMatrix * worldPosition;
	OutData.Position = worldPosition.xyz;
	OutData.ClipSpace = gl_Position;

	vec3 cameraPosition = (inverse(frg_ViewMatrix)[3]).xyz;
	OutData.ToCameraVector = cameraPosition - OutData.Position;
	OutData.TexCoord = in_TexCoord * 80.0;
	OutData.LightSpacePosition = frg_LightSpaceTransform * worldPosition;
}

#shader GEOMETRY
#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in VData
{
	vec3 Position;
	vec2 TexCoord;
	vec4 ClipSpace;
	vec3 ToCameraVector;
	vec4 LightSpacePosition;
} InData[];

out FData
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoord;
	vec4 ClipSpace;
	vec3 ToCameraVector;
	vec4 LightSpacePosition;
	mat3 TBN;
} OutData;

void main()
{
	vec3 v0 = InData[1].Position - InData[0].Position;
	vec3 v1 = InData[2].Position - InData[0].Position;
	vec3 normal = normalize(cross( v0, v1 ));
	vec3 tangent = normal;
	tangent.y = 0.0;
	if (length(tangent) < 0.00001) {
		tangent = vec3(1.0, 0.0, 0.0);
	}
	else
	{
		tangent = normalize(tangent);
	}
	vec3 bitangent = normalize(cross(normal, tangent));
	tangent = normalize(cross(bitangent, normal));
	mat3 tbn = mat3(tangent, normal, bitangent);
	for (int i = 0; i < gl_in.length(); i++)
	{
		gl_Position = gl_in[i].gl_Position;
		OutData.Position = InData[i].Position;
		OutData.Normal = normal;
		OutData.TexCoord = InData[i].TexCoord;
		OutData.ClipSpace = InData[i].ClipSpace;
		OutData.ToCameraVector = InData[i].ToCameraVector;
		OutData.LightSpacePosition = InData[i].LightSpacePosition;
		OutData.TBN = tbn;
		EmitVertex();
	}
}

#shader FRAGMENT
#version 430 core
#include "Lighting.h"
#include "Shadows.h"

out layout(location = 0) vec4 out_FragColor;

uniform vec4 u_Color;
uniform LightSource frg_LightSources[MAX_LIGHT_COUNT];
uniform int frg_UsedLightSources;
uniform samplerCube frg_ShadowMap;
uniform vec3 frg_LightPosition;
uniform vec3 frg_CameraPosition;

uniform sampler2D u_RefractionTexture;
uniform sampler2D u_ReflectionTexture;
uniform sampler2D u_DepthTexture;
uniform sampler2D u_NormalMap;
uniform sampler2D u_DUDVMap;
uniform sampler2D u_FoamTexture;
uniform float frg_FarPlane;
uniform float frg_NearPlane;
uniform float u_Time;

const float waveStrength = 0.05;

in FData
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoord;
	vec4 ClipSpace;
	vec3 ToCameraVector;
	vec4 LightSpacePosition;
	mat3 TBN;
} InData;

// const vec4 BLUE = vec4(0.0, 58.0 / 255.0, 78.0 / 255.0, 1.0);
const vec4 BLUE = vec4(0.1, 0.5, 0.8, 1.0);

float unprojectDepth(float depth)
{
	return 2.0 * frg_NearPlane * frg_FarPlane / (frg_FarPlane + frg_NearPlane - (2.0 * depth - 1.0) * (frg_FarPlane - frg_NearPlane));
}

void main()
{
	vec3 unitNormal = normalize(InData.Normal);
	vec3 unitToCamera = normalize(InData.ToCameraVector);
	vec2 textureCoords = InData.ClipSpace.xy / InData.ClipSpace.w / 2.0 + 0.5;
	vec2 refractCoords = vec2(textureCoords.x, textureCoords.y);
	vec2 reflectCoords = vec2(textureCoords.x, 1.0 - textureCoords.y);

	float depth = texture(u_DepthTexture, refractCoords).r;
	float floorDistance = unprojectDepth(depth);
	float waterDistance = unprojectDepth(gl_FragCoord.z);
	float waterDepth = floorDistance - waterDistance;

	vec2 distortedTextureCoords = texture(u_DUDVMap, vec2(InData.TexCoord.x + u_Time, InData.TexCoord.y)).rg * 0.1;
	distortedTextureCoords = InData.TexCoord + vec2(distortedTextureCoords.x, distortedTextureCoords.y + u_Time);
	vec2 totalDistortion = (texture(u_DUDVMap, distortedTextureCoords).rg * 2.0 - 1.0) * waveStrength * clamp(waterDepth / 5.0, 0.0, 1.0);

	vec4 normalMapValue = texture(u_NormalMap, distortedTextureCoords);
	vec3 worldNormal = InData.TBN * vec3(normalMapValue.r * 2.0 - 1.0, normalMapValue.b * 3.0, normalMapValue.g * 2.0 - 1.0);
	unitNormal = normalize(worldNormal);

	refractCoords += totalDistortion;
	refractCoords = clamp(refractCoords, 0.001, 0.999);
	reflectCoords += totalDistortion;
	reflectCoords = clamp(reflectCoords, 0.001, 0.999);

	vec4 refraction = texture(u_RefractionTexture, refractCoords);
	vec4 reflection = texture(u_ReflectionTexture, reflectCoords);

	float refractiveFactor = pow(abs(dot(unitToCamera, unitNormal)), 0.5);

	vec4 color = BLUE * mix(reflection, refraction, max(refractiveFactor, 0.0));

#ifdef SHADOW_MAP
	float shadow = calculatePointShadow(InData.Position, frg_ShadowMap, frg_FarPlane, frg_LightPosition, frg_CameraPosition);
#else
	float shadow = 0.0;
#endif

	vec4 lighting = calculateLightDiffuse(InData.Position, unitNormal, frg_LightSources, frg_UsedLightSources, shadow);
	lighting += calculateLightSpecular(InData.Position, unitNormal, 8.0, 10.0, unitToCamera, frg_LightSources, frg_UsedLightSources, shadow);
	out_FragColor = color * lighting;
	out_FragColor.a = clamp(waterDepth / 0.5, 0.1, 1.0);
}

#shader VERTEX
#version 430 core
#include "Clipping.h"

in layout(location = 0) vec3 in_Position;
in layout(location = 1) vec3 in_Normal;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ProjViewMatrix;
uniform vec4 u_ClippingPlanes[MAX_CLIPPING_PLANES];
uniform int u_UsedClippingPlanes;

uniform mat4 u_LightSpaceTransform;

out vec3 f_Position;
out vec3 f_Normal;
out vec4 f_LightSpacePosition;

void main()
{
	vec4 worldPosition = u_ModelMatrix * vec4(in_Position, 1.0);
	clipPlanes(worldPosition.xyz, u_ClippingPlanes, u_UsedClippingPlanes);
	gl_Position = u_ProjViewMatrix * worldPosition;
	f_Normal = (transpose(inverse(u_ModelMatrix)) * vec4(in_Normal, 0.0)).xyz;
	f_Position = worldPosition.xyz;

	f_LightSpacePosition = u_LightSpaceTransform * worldPosition;
}

#shader FRAGMENT
#version 430 core
#include "Lighting.h"
#include "Shadows.h"

out layout(location = 0) vec4 out_FragColor;

const vec3 WORLD_UP = vec3(0.0, 1.0, 0.0);
const vec4 GRASS_COLOR = vec4(96 / 255.0, 128 / 255.0, 56 / 255.0, 1.0);
const vec4 STONE_COLOR = vec4(0.7608, 0.6235, 0.4588, 1.0);
const vec4 SAND_COLOR = vec4(0.761, 0.698, 0.502, 1.0);

const float FLATNESS_PROPORTION = 0.6;

uniform vec4 u_Color;
uniform LightSource u_LightSources[MAX_LIGHT_COUNT];
uniform int u_UsedLightSources;
uniform samplerCube u_ShadowMap;
uniform float u_FarPlane;
uniform vec3 u_LightPosition;
uniform vec3 u_CameraPosition;

in vec3 f_Position;
in vec3 f_Normal;
in vec4 f_LightSpacePosition;

void main()
{
#ifdef SHADOW_MAP
	float shadow = calculatePointShadow(f_Position, u_ShadowMap, u_FarPlane, u_LightPosition, u_CameraPosition);
#else
	float shadow = 0.0;
#endif
	float flatness = dot(WORLD_UP, f_Normal);
	vec4 baseColor = u_Color;
	if (f_Position.y > 0 && flatness >= FLATNESS_PROPORTION)
	{
		baseColor = mix(baseColor, GRASS_COLOR, max((flatness - FLATNESS_PROPORTION) / (1.0 - FLATNESS_PROPORTION), 0.5));
	}
	out_FragColor = baseColor * calculateLightDiffuse(f_Position, f_Normal, u_LightSources, u_UsedLightSources, shadow);
}

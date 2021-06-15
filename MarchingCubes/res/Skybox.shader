#shader VERTEX
#version 430 core
#include "Clipping.h"

in layout(location = 0) vec3 in_Position;

uniform mat4 frg_ModelMatrix;
uniform mat4 frg_ViewMatrix;
uniform mat4 frg_ProjectionMatrix;
uniform vec4 frg_ClippingPlanes[MAX_CLIPPING_PLANES];
uniform int frg_UsedClippingPlanes;

out vec3 f_TexCoord;

void main()
{
	vec4 worldPosition = frg_ModelMatrix * vec4(in_Position, 1.0);
	clipPlanes(worldPosition.xyz, frg_ClippingPlanes, frg_UsedClippingPlanes);

	mat4 viewMatrix = frg_ViewMatrix;
	viewMatrix[3] = vec4(0.0, 0.0, 0.0, viewMatrix[3][3]);

	gl_Position = frg_ProjectionMatrix * viewMatrix * worldPosition;
	f_TexCoord = in_Position;
}

#shader FRAGMENT
#version 430 core

out layout(location = 0) vec4 out_FragColor;

uniform samplerCube u_Texture;

in vec3 f_TexCoord;

void main()
{
	out_FragColor = texture(u_Texture, f_TexCoord);
}

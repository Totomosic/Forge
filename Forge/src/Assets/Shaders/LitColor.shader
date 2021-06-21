#shader VERTEX
#version 450 core
#include <Clipping.h>
layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec3 v_Normal;

uniform mat4 frg_ModelMatrix;
uniform mat4 frg_ProjViewMatrix;
uniform vec4 frg_ClippingPlanes[MAX_CLIPPING_PLANES];
uniform int frg_UsedClippingPlanes;

out vec3 f_Position;
out vec3 f_Normal;

void main()
{
    vec4 worldPosition = frg_ModelMatrix * vec4(v_Position, 1.0);
    clipPlanes(worldPosition.xyz, frg_ClippingPlanes, frg_UsedClippingPlanes);
    gl_Position = frg_ProjViewMatrix * worldPosition;
    f_Position = worldPosition.xyz;
    f_Normal = vec3(transpose(inverse(frg_ModelMatrix)) * vec4(v_Normal, 0.0));
}

#shader FRAGMENT
#version 450 core
#include <Lighting.h>

layout (location = 0) out vec4 f_FinalColor;

["Color"]
uniform vec4 u_Color;
uniform vec3 frg_CameraPosition;

in vec3 f_Position;
in vec3 f_Normal;

void main()
{
#ifdef NO_LIGHTING
    f_FinalColor = u_Color;
#else
    MaterialOptions material;
    material.Diffuse = 1.0;
    material.Specular = 0.0;
    material.ShineDamper = 0.0;
    f_FinalColor = u_Color * CalculateLighting(f_Position, normalize(f_Normal), frg_CameraPosition, material);
#endif
}

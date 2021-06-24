#shader VERTEX
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
#include <PBR.h>

layout (location = 0) out vec4 f_FinalColor;

["Albedo"]
uniform vec4 u_Albedo;
["Roughness"]
uniform float u_Roughness;
["Metallic"]
uniform float u_Metallic;
["AO"]
uniform float u_AO;
uniform vec3 frg_CameraPosition;

in vec3 f_Position;
in vec3 f_Normal;

void main()
{
#ifdef NO_LIGHTING
    f_FinalColor = u_Albedo;
#else
    PBRMaterialOptions material;
    material.Albedo = u_Albedo;
    material.Roughness = u_Roughness;
    material.Metallic = u_Metallic;
    material.AmbientOcclusion = u_AO;
    f_FinalColor = CalculateLightingPBR(f_Position, normalize(f_Normal), frg_CameraPosition, material);
#endif
}

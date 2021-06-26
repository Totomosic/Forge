#shader VERTEX
#include <Clipping.h>
layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec3 v_Normal;
layout (location = 2) in vec2 v_TexCoord;

uniform mat4 frg_ModelMatrix;
uniform mat4 frg_ProjViewMatrix;
uniform vec4 frg_ClippingPlanes[MAX_CLIPPING_PLANES];
uniform int frg_UsedClippingPlanes;

out vec3 f_Position;
out vec3 f_Normal;
out vec2 f_TexCoord;

void main()
{
    vec4 worldPosition = frg_ModelMatrix * vec4(v_Position, 1.0);
    clipPlanes(worldPosition.xyz, frg_ClippingPlanes, frg_UsedClippingPlanes);
    gl_Position = frg_ProjViewMatrix * worldPosition;
    f_Position = vec3(frg_ModelMatrix * vec4(v_Position, 1.0));
    f_Normal = vec3(transpose(inverse(frg_ModelMatrix)) * vec4(v_Normal, 0.0));
    f_TexCoord = v_TexCoord;
}

#shader FRAGMENT
#include <Lighting.h>

layout (location = 0) out vec4 f_FinalColor;

["Texture"]
uniform sampler2D u_Texture;
uniform vec3 frg_CameraPosition;

in vec3 f_Position;
in vec3 f_Normal;
in vec2 f_TexCoord;

void main()
{
#ifdef NO_LIGHTING
    f_FinalColor = texture(u_Texture, f_TexCoord);
#else
    MaterialOptions material;
    material.Diffuse = 1.0;
    material.Specular = 0.0;
    material.ShineDamper = 0.0;
    f_FinalColor = texture(u_Texture, f_TexCoord) * CalculateLighting(f_Position, normalize(f_Normal), frg_CameraPosition, material);
#endif
}
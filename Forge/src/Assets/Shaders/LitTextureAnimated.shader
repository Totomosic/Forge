#shader VERTEX
#version 450 core
layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec3 v_Normal;
layout (location = 2) in vec2 v_TexCoord;
layout (location = 4) in ivec4 v_JointIds;
layout (location = 5) in vec4 v_JointWeights;

uniform mat4 frg_ModelMatrix;
uniform mat4 frg_ProjViewMatrix;

uniform mat4 frg_JointTransforms[JOINT_COUNT];

out vec3 f_Position;
out vec3 f_Normal;
out vec2 f_TexCoord;

void main()
{
    vec4 localPosition = vec4(0.0);
    vec4 normal = vec4(0.0);

    for (int i = 0; i < 4; i++)
    {
        mat4 jointTransform = frg_JointTransforms[v_JointIds[i]];
        vec4 posePosition = jointTransform * vec4(v_Position, 1.0);
        localPosition += posePosition * v_JointWeights[i];

        vec4 worldNormal = jointTransform * vec4(v_Normal, 0.0);
        normal += worldNormal * v_JointWeights[i];
    }

    gl_Position = frg_ProjViewMatrix * frg_ModelMatrix * localPosition;
    f_Position = vec3(frg_ModelMatrix * localPosition);
    f_Normal = normalize(normal.xyz);
    f_TexCoord = v_TexCoord;
}

#shader FRAGMENT
#version 450 core
#include <Lighting.h>

layout (location = 0) out vec4 f_FinalColor;

["Texture"]
uniform sampler2D u_Texture;
uniform LightSource frg_LightSources[MAX_LIGHT_COUNT];
uniform int frg_UsedLightSources;

in vec3 f_Position;
in vec3 f_Normal;
in vec2 f_TexCoord;

void main()
{
    float shadow = 0.0;
    f_FinalColor = texture(u_Texture, f_TexCoord) * calculateLightDiffuse(f_Position, normalize(f_Normal), frg_LightSources, frg_UsedLightSources, shadow);
}

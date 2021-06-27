#shader VERTEX
layout (location = 0) in vec3 v_Position;
layout (location = 4) in ivec4 v_JointIds;
layout (location = 5) in vec4 v_JointWeights;

uniform mat4 frg_ModelMatrix;

layout(std140, binding = 0) uniform Camera
{
    mat4 frg_ViewMatrix;
    mat4 frg_ProjectionMatrix;
    mat4 frg_ProjViewMatrix;
    float frg_FarPlane;
    float frg_NearPlane;
    vec3 frg_CameraPosition;
};

uniform mat4 frg_JointTransforms[JOINT_COUNT];

void main()
{
    vec4 localPosition = vec4(0.0);

    for (int i = 0; i < 4; i++)
    {
        mat4 jointTransform = frg_JointTransforms[v_JointIds[i]];
        vec4 posePosition = jointTransform * vec4(v_Position, 1.0);
        localPosition += posePosition * v_JointWeights[i];
    }

    gl_Position = frg_ProjViewMatrix * frg_ModelMatrix * localPosition;
}

#shader FRAGMENT
layout (location = 0) out vec4 f_FinalColor;

["Color"]
uniform vec4 u_Color;

void main()
{
    f_FinalColor = u_Color;
}

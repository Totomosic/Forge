#shader vertex
#version 450 core

layout (location = 0) in vec3 v_Position;

void main()
{
    gl_Position = vec4(v_Position, 1.0);
}

#shader fragment
#version 450 core

layout (location = 0) out vec4 f_FinalColor;

uniform vec4 u_Color;

void main()
{
    f_FinalColor = u_Color;
}

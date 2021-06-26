#shader VERTEX
layout (location = 0) in vec3 v_Position;

uniform mat4 frg_ModelMatrix;
uniform mat4 frg_ProjViewMatrix;

void main()
{
    gl_Position = frg_ProjViewMatrix * frg_ModelMatrix * vec4(v_Position, 1.0);
}

#shader FRAGMENT
void main()
{
}
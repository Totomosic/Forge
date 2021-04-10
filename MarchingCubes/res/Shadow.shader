#shader VERTEX
#version 430 core

in layout(location = 0) vec3 v_Position;
in layout(location = 2) vec2 v_TexCoord;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ProjViewMatrix;

out vec2 f_TexCoord;

void main()
{
	gl_Position = u_ProjViewMatrix * u_ModelMatrix * vec4(v_Position, 1.0);
	f_TexCoord = v_TexCoord;
}

#shader FRAGMENT
#version 430 core

out layout(location = 0) vec4 out_FragColor;

uniform sampler2D u_ShadowMap;

in vec2 f_TexCoord;

void main()
{
	float depth = texture(u_ShadowMap, f_TexCoord).r;
	out_FragColor = vec4(vec3(depth), 1.0);
}

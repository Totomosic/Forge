#shader VERTEX
in layout(location = 0) vec3 v_Position;
in layout(location = 2) vec2 v_TexCoord;

out vec2 f_TexCoord;

void main()
{
	gl_Position = vec4(v_Position * 2.0, 1.0);
	f_TexCoord = v_TexCoord;
}

#shader FRAGMENT
out layout(location = 0) vec4 out_FragColor;

uniform float u_Depth;
uniform sampler2D u_Texture;

in vec2 f_TexCoord;

void main()
{
	out_FragColor = texture(u_Texture, f_TexCoord);
	if (u_Depth > 0)
	{
		out_FragColor.xyz = mix(out_FragColor.xyz, vec3(0.1, 0.1, 0.5), clamp(u_Depth / 0.5, 0.0, 0.75));
	}
}

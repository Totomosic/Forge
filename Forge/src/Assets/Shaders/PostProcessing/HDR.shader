#shader VERTEX
layout(location = 0) in vec3 v_Position;
layout(location = 2) in vec2 v_TexCoord;

out vec2 f_TexCoord;

void main()
{
    gl_Position = vec4(v_Position * 2.0, 1.0);
    f_TexCoord = v_TexCoord;
}

#shader FRAGMENT
layout(location = 0) out vec4 f_FragColor;

in vec2 f_TexCoord;

uniform sampler2D frg_Texture;
["Exposure"]
uniform float u_Exposure;

void main()
{
    vec4 color = texture(frg_Texture, f_TexCoord);
    vec3 mapped = vec3(1.0) - exp(-color.xyz * u_Exposure);
    mapped = pow(mapped, vec3(1.0 / 2.2));
    f_FragColor = vec4(mapped, 1.0);
}

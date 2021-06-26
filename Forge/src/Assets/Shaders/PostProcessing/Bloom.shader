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
layout(location = 1) out vec4 f_BrightColor;

in vec2 f_TexCoord;

uniform sampler2D frg_Texture;
["BloomThreshold"]
uniform float u_Threshold;

void main()
{
    vec4 color = texture(frg_Texture, f_TexCoord);
    float brightness = dot(color.xyz, vec3(0.2126, 0.7152, 0.0722));
    if (brightness >= u_Threshold)
        f_BrightColor = vec4(color.xyz, 1.0);
    else
        f_BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    f_FragColor = color;
}

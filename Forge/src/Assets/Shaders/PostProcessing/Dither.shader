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
uniform sampler2D frg_BayerMatrix;

void main()
{
    vec4 color = texture(frg_Texture, f_TexCoord);
    float bayer = texture(frg_BayerMatrix, gl_FragCoord.xy / 8.0).r * (255.0 / 64.0);
    vec4 rgba = color * 255.0;
    vec4 head = floor(rgba);
    vec4 tail = rgba - head;
    vec4 finalColor = head + step(bayer, tail);
    f_FragColor = finalColor / 255.0;
}

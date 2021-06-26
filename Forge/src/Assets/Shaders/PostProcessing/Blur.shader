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

uniform bool u_Horizontal;
const float weights[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 texOffset = 1.0 / textureSize(frg_Texture, 0);
    vec3 result = texture(frg_Texture, f_TexCoord).rgb * weights[0];
    if (u_Horizontal)
    {
        for (int i = 1; i < 5; ++i)
        {
            result += texture(frg_Texture, f_TexCoord + vec2(texOffset.x * i, 0.0)).rgb * weights[i];
            result += texture(frg_Texture, f_TexCoord - vec2(texOffset.x * i, 0.0)).rgb * weights[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; ++i)
        {
            result += texture(frg_Texture, f_TexCoord + vec2(0.0, texOffset.y * i)).rgb * weights[i];
            result += texture(frg_Texture, f_TexCoord - vec2(0.0, texOffset.y * i)).rgb * weights[i];
        }
    }
    f_FragColor = vec4(result, 1.0);
}

#shader VERTEX
layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec2 v_TexCoord;
layout(location = 2) in vec4 v_Color;
layout(location = 3) in int v_TextureID;

layout(std140, binding = 0) uniform Camera
{
    mat4 frg_ProjectionMatrix;
    mat4 frg_ViewMatrix;
    mat4 frg_ViewProjectionMatrix;
};

out vec2 f_TexCoord;
out vec4 f_Color;
out flat int f_TextureID;

void main()
{
    gl_Position = frg_ViewProjectionMatrix * vec4(v_Position, 1.0);
    f_TexCoord = v_TexCoord;
    f_Color = v_Color;
    f_TextureID = v_TextureID;
}

#shader FRAGMENT
layout(location = 0) out vec4 f_FragColor;

in vec2 f_TexCoord;
in vec4 f_Color;
in flat int f_TextureID;

uniform sampler2D u_Textures[32];

void main()
{
    f_FragColor = texture(u_Textures[f_TextureID], f_TexCoord) * f_Color;
}

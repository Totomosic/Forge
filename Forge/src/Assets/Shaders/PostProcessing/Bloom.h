std::string vertexShaderSource =
	"layout(location = 0) in vec3 v_Position;\n"
	"layout(location = 2) in vec2 v_TexCoord;\n"
	"\n"
	"out vec2 f_TexCoord;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    gl_Position = vec4(v_Position * 2.0, 1.0);\n"
	"    f_TexCoord = v_TexCoord;\n"
	"}\n"
	"\n";
std::string fragmentShaderSource =
	"layout(location = 0) out vec4 f_FragColor;\n"
	"layout(location = 1) out vec4 f_BrightColor;\n"
	"\n"
	"in vec2 f_TexCoord;\n"
	"\n"
	"uniform sampler2D frg_Texture;\n"
	"[\"BloomThreshold\"]\n"
	"uniform float u_Threshold;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    vec4 color = texture(frg_Texture, f_TexCoord);\n"
	"    float brightness = dot(color.xyz, vec3(0.2126, 0.7152, 0.0722));\n"
	"    if (brightness >= u_Threshold)\n"
	"        f_BrightColor = vec4(color.xyz, 1.0);\n"
	"    else\n"
	"        f_BrightColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
	"    f_FragColor = color;\n"
	"}\n";
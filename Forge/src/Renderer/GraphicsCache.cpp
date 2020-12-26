#include "ForgePch.h"
#include "GraphicsCache.h"
#include "Layout.h"

namespace Forge
{

    static std::string SHADER_VERSION_STRING = "#version 450 core";

    Ref<Shader> GraphicsCache::s_DefaultColorShader;
    Ref<Shader> GraphicsCache::s_DefaultTextureShader;
    Ref<Shader> GraphicsCache::s_LitColorShader;
    Ref<Shader> GraphicsCache::s_LitTextureShader;

    Ref<Mesh> GraphicsCache::s_SquareMesh;
    Ref<Mesh> GraphicsCache::s_CubeMesh;

    void GraphicsCache::Init()
    {
        CreateDefaultColorShader();
        CreateDefaultTextureShader();
        CreateLitColorShader();
        CreateLitTextureShader();

        CreateSquareMesh();
        CreateCubeMesh();
    }

    Ref<Material> GraphicsCache::DefaultColorMaterial(const Color& color)
    {
        Ref<Material> material = CreateRef<Material>(DefaultColorShader());
        material->GetUniforms().AddUniform("u_Color", color);
        return material;
    }

    Ref<Material> GraphicsCache::DefaultTextureMaterial(const Ref<Texture>& texture)
    {
        Ref<Material> material = CreateRef<Material>(DefaultTextureShader());
        material->GetUniforms().AddUniform("u_Texture", texture);
        return material;
    }

    Ref<Material> GraphicsCache::LitColorMaterial(const Color& color)
    {
        Ref<Material> material = CreateRef<Material>(LitColorShader());
        material->GetUniforms().AddUniform("u_Color", color);
        return material;
    }

    Ref<Material> GraphicsCache::LitTextureMaterial(const Ref<Texture>& texture)
    {
        Ref<Material> material = CreateRef<Material>(LitTextureShader());
        material->GetUniforms().AddUniform("u_Texture", texture);
        return material;
    }

    void GraphicsCache::CreateDefaultColorShader()
    {
        std::string vertexShaderSource =
            SHADER_VERSION_STRING + '\n' +
            "layout (location = 0) in vec3 v_Position;\n"
            "\n"
            "uniform mat4 u_ModelMatrix;\n"
            "uniform mat4 u_ProjViewMatrix;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    gl_Position = u_ProjViewMatrix * u_ModelMatrix * vec4(v_Position, 1.0);\n"
            "}\n";

        std::string fragmentShaderSource =
            SHADER_VERSION_STRING + '\n' +
            "layout (location = 0) out vec4 f_FinalColor;\n"
            "\n"
            "uniform vec4 u_Color;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    f_FinalColor = u_Color;\n"
            "}\n";

        s_DefaultColorShader = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource);
    }

    void GraphicsCache::CreateDefaultTextureShader()
    {
        std::string vertexShaderSource =
            SHADER_VERSION_STRING + '\n' +
            "layout (location = 0) in vec3 v_Position;\n"
            "layout (location = 2) in vec2 v_TexCoord;\n"
            "\n"
            "uniform mat4 u_ModelMatrix;\n"
            "uniform mat4 u_ProjViewMatrix;\n"
            "\n"
            "out vec2 f_TexCoord;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    gl_Position = u_ProjViewMatrix * u_ModelMatrix * vec4(v_Position, 1.0);\n"
            "    f_TexCoord = v_TexCoord;\n"
            "}\n";

        std::string fragmentShaderSource =
            SHADER_VERSION_STRING + '\n' +
            "layout (location = 0) out vec4 f_FinalColor;\n"
            "\n"
            "uniform sampler2D u_Texture;\n"
            "\n"
            "in vec2 f_TexCoord;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    f_FinalColor = texture(u_Texture, f_TexCoord);\n"
            "}\n";

        s_DefaultTextureShader = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource);
    }

    void GraphicsCache::CreateLitColorShader()
    {
        std::string vertexShaderSource =
            SHADER_VERSION_STRING + '\n' +
            "layout (location = 0) in vec3 v_Position;\n"
            "layout (location = 1) in vec3 v_Normal;\n"
            "\n"
            "uniform mat4 u_ModelMatrix;\n"
            "uniform mat4 u_ProjViewMatrix;\n"
            "\n"
            "out vec3 f_Position;\n"
            "out vec3 f_Normal;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    gl_Position = u_ProjViewMatrix * u_ModelMatrix * vec4(v_Position, 1.0);\n"
            "    f_Position = vec3(u_ModelMatrix * vec4(v_Position, 1.0));\n"
            "    f_Normal = vec3(transpose(inverse(u_ModelMatrix)) * vec4(v_Normal, 0.0));\n"
            "}\n";

        std::string fragmentShaderSource =
            SHADER_VERSION_STRING + '\n' +
            "#include <Lighting.h>\n"
            "\n"
            "layout (location = 0) out vec4 f_FinalColor;\n"
            "\n"
            "uniform vec4 u_Color;\n"
            "uniform LightSource u_LightSources[MAX_LIGHT_COUNT];\n"
            "uniform int u_UsedLightSources;\n"
            "\n"
            "in vec3 f_Position;\n"
            "in vec3 f_Normal;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    vec3 color = vec3(0.0);\n"
            "\n"
            "    for (int i = 0; i < u_UsedLightSources; i++)\n"
            "    {\n"
            "        vec3 lightDirection = normalize(u_LightSources[i].Position - f_Position);\n"
            "        vec3 normal = normalize(f_Normal);\n"
            "        float diffusePower = max(dot(normal, lightDirection), 0.0);\n"
            "        vec4 diffuseColor = diffusePower * u_LightSources[i].Color;\n"
            "        color += diffuseColor.xyz + u_LightSources[i].Ambient * u_LightSources[i].Color.xyz;\n"
            "    }\n"
            "\n"
            "    f_FinalColor = vec4(color, 1.0) * u_Color;\n"
            "}\n";

        s_LitColorShader = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource);
    }

    void GraphicsCache::CreateLitTextureShader()
    {
        std::string vertexShaderSource =
            SHADER_VERSION_STRING + '\n' +
            "layout (location = 0) in vec3 v_Position;\n"
            "layout (location = 1) in vec3 v_Normal;\n"
            "layout (location = 2) in vec2 v_TexCoord;\n"
            "\n"
            "uniform mat4 u_ModelMatrix;\n"
            "uniform mat4 u_ProjViewMatrix;\n"
            "\n"
            "out vec3 f_Position;\n"
            "out vec3 f_Normal;\n"
            "out vec2 f_TexCoord;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    gl_Position = u_ProjViewMatrix * u_ModelMatrix * vec4(v_Position, 1.0);\n"
            "    f_Position = vec3(u_ModelMatrix * vec4(v_Position, 1.0));\n"
            "    f_Normal = vec3(transpose(inverse(u_ModelMatrix)) * vec4(v_Normal, 0.0));\n"
            "    f_TexCoord = v_TexCoord;\n"
            "}\n";

        std::string fragmentShaderSource =
            SHADER_VERSION_STRING + '\n' +
            "#include <Lighting.h>\n"
            "\n"
            "layout (location = 0) out vec4 f_FinalColor;\n"
            "\n"
            "uniform sampler2D u_Texture;\n"
            "uniform LightSource u_LightSources[MAX_LIGHT_COUNT];\n"
            "uniform int u_UsedLightSources;\n"
            "\n"
            "in vec3 f_Position;\n"
            "in vec3 f_Normal;\n"
            "in vec2 f_TexCoord;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    vec3 color = vec3(0.0);\n"
            "\n"
            "    for (int i = 0; i < u_UsedLightSources; i++)\n"
            "    {\n"
            "        vec3 lightDirection = normalize(u_LightSources[i].Position - f_Position);\n"
            "        vec3 normal = normalize(f_Normal);\n"
            "        float diffusePower = max(dot(normal, lightDirection), 0.0);\n"
            "        vec4 diffuseColor = diffusePower * u_LightSources[i].Color;\n"
            "        color += diffuseColor.xyz + u_LightSources[i].Ambient * u_LightSources[i].Color.xyz;\n"
            "    }\n"
            "\n"
            "    f_FinalColor = vec4(color, 1.0) * texture(u_Texture, f_TexCoord);\n"
            "}\n";

        s_LitTextureShader = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource);
    }

    void GraphicsCache::CreateSquareMesh()
    {
        float vertices[] = {
            -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        };

        uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

        BufferLayout layout({
            { ShaderDataType::Float3 },
            { ShaderDataType::Float3 },
            { ShaderDataType::Float2 },
        });

        Ref<VertexBuffer> vbo = VertexBuffer::Create(vertices, sizeof(vertices), layout);
        Ref<IndexBuffer> ibo = IndexBuffer::Create(indices, sizeof(indices));
        Ref<VertexArray> vao = VertexArray::Create();
        vao->AddVertexBuffer(vbo);
        vao->SetIndexBuffer(ibo);

        s_SquareMesh = CreateRef<Mesh>(vao);
    }

    void GraphicsCache::CreateCubeMesh()
    {
        uint32_t indices[] = { 0, 1, 2, 0, 2, 3,  11, 10, 13, 11, 13, 12,  4, 5, 6, 4, 6, 7,  15, 14, 9, 15, 9, 8,  23, 16, 19, 23, 19, 20,  17, 22, 21, 17, 21, 18 };

        float vertices[] = {
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -0.5, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,

            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        };

        BufferLayout layout({
            { ShaderDataType::Float3 },
            { ShaderDataType::Float3 },
            { ShaderDataType::Float2 },
        });

        Ref<VertexBuffer> vbo = VertexBuffer::Create(vertices, sizeof(vertices), layout);
        Ref<IndexBuffer> ibo = IndexBuffer::Create(indices, sizeof(indices));
        Ref<VertexArray> vao = VertexArray::Create();
        vao->AddVertexBuffer(vbo);
        vao->SetIndexBuffer(ibo);

        s_CubeMesh = CreateRef<Mesh>(vao);
    }

}

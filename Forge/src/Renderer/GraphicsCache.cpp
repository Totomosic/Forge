#include "ForgePch.h"
#include "GraphicsCache.h"
#include "Layout.h"

namespace Forge
{

    static std::string SHADER_VERSION_STRING = "#version 450 core";

    Ref<Shader> GraphicsCache::s_DefaultColorShader;

    Ref<Mesh> GraphicsCache::s_SquareMesh;

    void GraphicsCache::Init()
    {
        CreateDefaultColorShader();

        CreateSquareMesh();
    }

    Ref<Material> GraphicsCache::DefaultColorMaterial(const Color& color)
    {
        Ref<Material> material = CreateRef<Material>(DefaultColorShader());
        material->GetUniforms().AddUniform("u_Color", color);
        return material;
    }

    void GraphicsCache::CreateDefaultColorShader()
    {
        std::string vertexShaderSource =
            SHADER_VERSION_STRING + '\n' +
            "layout (location = 0) in vec3 v_Position;\n"
            "uniform mat4 u_ModelMatrix;\n"
            "uniform mat4 u_ProjViewMatrix;\n"
            "void main()\n"
            "{\n"
            "gl_Position = u_ProjViewMatrix * u_ModelMatrix * vec4(v_Position, 1.0);\n"
            "}\n";

        std::string fragmentShaderSource =
            SHADER_VERSION_STRING + '\n' +
            "layout (location = 0) out vec4 f_FinalColor;\n"
            "uniform vec4 u_Color;\n"
            "void main()\n"
            "{\n"
            "f_FinalColor = u_Color;\n"
            "}\n";

        s_DefaultColorShader = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource);
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

}

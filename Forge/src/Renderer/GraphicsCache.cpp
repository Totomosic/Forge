#include "ForgePch.h"
#include "GraphicsCache.h"
#include "Layout.h"

namespace Forge
{

    static std::string SHADER_VERSION_STRING = "#version 450 core";

    Ref<Shader> GraphicsCache::s_DefaultColorShader;
    Ref<Shader> GraphicsCache::s_DefaultTextureShader;
    Ref<Shader> GraphicsCache::s_LitColorShader[2];
    Ref<Shader> GraphicsCache::s_LitTextureShader[2];
    Ref<Shader> GraphicsCache::s_DefaultShadowShader;
    Ref<Shader> GraphicsCache::s_DefaultPointShadowShader;
    std::unordered_map<int, Ref<Shader>> GraphicsCache::s_DefaultColorAnimatedShaders;
    std::unordered_map<int, Ref<Shader>> GraphicsCache::s_LitTextureAnimatedShaders;

    Ref<Mesh> GraphicsCache::s_SquareMesh;
    Ref<Mesh> GraphicsCache::s_CubeMesh;

    void GraphicsCache::Init()
    {
        CreateDefaultColorShader();
        CreateDefaultTextureShader();
        CreateLitColorShader();
        CreateLitTextureShader();
        CreateDefaultShadowShader();
        CreateDefaultPointShadowShader();

        CreateSquareMesh();
        CreateCubeMesh();
    }

    Ref<Mesh> GraphicsCache::GridMesh(int xVertices, int zVertices)
    {
        FORGE_ASSERT(xVertices >= 2, "Invalid x vertex count");
        FORGE_ASSERT(zVertices >= 2, "Invalid z vertex count");
        float xSpacing = 1.0f / (xVertices - 1.0f);
        float zSpacing = 1.0f / (zVertices - 1.0f);

        uint32_t vertexCount = xVertices * zVertices;
        uint32_t indexCount = (xVertices - 1) * (zVertices - 1) * 6;

        BufferLayout layout({
            { ShaderDataType::Float3 },
            { ShaderDataType::Float3 },
            { ShaderDataType::Float2 },
        });

        float* vertexData = new float[size_t(vertexCount) * 8];
        uint32_t* indexData = new uint32_t[indexCount];

        uint32_t index = 0;
        for (int z = 0; z < zVertices; z++)
        {
            for (int x = 0; x < xVertices; x++)
            {
                vertexData[index + 0] = x * xSpacing - 0.5f;
                vertexData[index + 1] = 0;
                vertexData[index + 2] = z * zSpacing - 0.5f;

                vertexData[index + 3] = 0;
                vertexData[index + 4] = 1;
                vertexData[index + 5] = 0;

                vertexData[index + 6] = x * xSpacing;
                vertexData[index + 7] = z * zSpacing;
                index += 8;
            }
        }

        index = 0;
        for (int z = 0; z < zVertices - 1; z++)
        {
            for (int x = 0; x < xVertices - 1; x++)
            {
                indexData[index + 0] = (x + 0) + (z + 0) * xVertices;
                indexData[index + 1] = (x + 0) + (z + 1) * xVertices;
                indexData[index + 2] = (x + 1) + (z + 1) * xVertices;

                indexData[index + 3] = (x + 0) + (z + 0) * xVertices;
                indexData[index + 4] = (x + 1) + (z + 1) * xVertices;
                indexData[index + 5] = (x + 1) + (z + 0) * xVertices;
                index += 6;
            }
        }

        Ref<VertexBuffer> vbo = VertexBuffer::Create(vertexData, vertexCount * layout.GetStride(), layout);
        Ref<IndexBuffer> ibo = IndexBuffer::Create(indexData, indexCount * sizeof(uint32_t));
        Ref<VertexArray> vao = VertexArray::Create();
        vao->AddVertexBuffer(vbo);
        vao->SetIndexBuffer(ibo);

        delete[] vertexData;
        delete[] indexData;

        return CreateRef<Mesh>(vao);
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
        Ref<Material> material = CreateRef<Material>(std::array<Ref<Shader>, 3>{ DefaultPointShadowShader(), LitColorShader(true), LitColorShader(false) });
        material->GetUniforms().AddUniform("u_Color", color);
        return material;
    }

    Ref<Material> GraphicsCache::LitTextureMaterial(const Ref<Texture>& texture)
    {
        Ref<Material> material = CreateRef<Material>(std::array<Ref<Shader>, 3>{ DefaultPointShadowShader(), LitTextureShader(true), LitTextureShader(false) });
        material->GetUniforms().AddUniform("u_Texture", texture);
        return material;
    }

    Ref<Material> GraphicsCache::AnimatedDefaultColorMaterial(int maxJoints, const Color& color)
    {
        Ref<Material> material = CreateRef<Material>(AnimatedDefaultColorShader(maxJoints));
        material->GetUniforms().AddUniform("u_Color", color);
        return material;
    }

    Ref<Material> GraphicsCache::AnimatedLitTextureMaterial(int maxJoints, const Ref<Texture>& texture)
    {
        Ref<Material> material = CreateRef<Material>(AnimatedLitTextureShader(maxJoints));
        material->GetUniforms().AddUniform("u_Texture", texture);
        return material;
    }

    void GraphicsCache::CreateDefaultColorShader()
    {
        std::string vertexShaderSource =
            SHADER_VERSION_STRING + '\n' +
            "#include \"Clipping.h\"\n"
            "layout (location = 0) in vec3 v_Position;\n"
            "\n"
            "uniform mat4 u_ModelMatrix;\n"
            "uniform mat4 u_ProjViewMatrix;\n"
            "uniform vec4 u_ClippingPlanes[MAX_CLIPPING_PLANES];\n"
            "uniform int u_UsedClippingPlanes;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    vec4 worldPosition = u_ModelMatrix * vec4(v_Position, 1.0);\n"
            "    clipPlanes(worldPosition.xyz, u_ClippingPlanes, u_UsedClippingPlanes);\n"
            "    gl_Position = u_ProjViewMatrix * worldPosition;\n"
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
            "#include \"Clipping.h\"\n"
            "layout (location = 0) in vec3 v_Position;\n"
            "layout (location = 1) in vec3 v_Normal;\n"
            "\n"
            "uniform mat4 u_ModelMatrix;\n"
            "uniform mat4 u_ProjViewMatrix;\n"
            "uniform vec4 u_ClippingPlanes[MAX_CLIPPING_PLANES];\n"
            "uniform int u_UsedClippingPlanes;\n"
            "\n"
            "out vec3 f_Position;\n"
            "out vec3 f_Normal;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    vec4 worldPosition = u_ModelMatrix * vec4(v_Position, 1.0);\n"
            "    clipPlanes(worldPosition.xyz, u_ClippingPlanes, u_UsedClippingPlanes);\n"
            "    gl_Position = u_ProjViewMatrix * worldPosition;\n"
            "    f_Position = worldPosition.xyz;\n"
            "    f_Normal = vec3(transpose(inverse(u_ModelMatrix)) * vec4(v_Normal, 0.0));\n"
            "}\n";

        std::string fragmentShaderSource =
            SHADER_VERSION_STRING + '\n' +
            "#include <Lighting.h>\n"
            "#include <Shadows.h>\n"
            "\n"
            "layout (location = 0) out vec4 f_FinalColor;\n"
            "\n"
            "uniform vec4 u_Color;\n"
            "uniform LightSource u_LightSources[MAX_LIGHT_COUNT];\n"
            "uniform int u_UsedLightSources;\n"
            "uniform samplerCube u_ShadowMap;\n"
            "uniform float u_FarPlane;\n"
            "uniform vec3 u_LightPosition;\n"
            "uniform vec3 u_CameraPosition;\n"
            "\n"
            "in vec3 f_Position;\n"
            "in vec3 f_Normal;\n"
            "\n"
            "void main()\n"
            "{\n"
            "#ifdef SHADOW_MAP\n"
            "    float shadow = calculatePointShadow(f_Position, u_ShadowMap, u_FarPlane, u_LightPosition, u_CameraPosition);\n"
            "#else\n"
            "    float shadow = 0.0;\n" 
            "#endif\n"
            "    f_FinalColor = u_Color * calculateLightDiffuse(f_Position, normalize(f_Normal), u_LightSources, u_UsedLightSources, shadow);\n"
            "}\n";

        s_LitColorShader[0] = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource);
        s_LitColorShader[1] = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource, ShaderDefines{ "SHADOW_MAP" });
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
            "#include <Shadows.h>\n"
            "\n"
            "layout (location = 0) out vec4 f_FinalColor;\n"
            "\n"
            "uniform sampler2D u_Texture;\n"
            "uniform LightSource u_LightSources[MAX_LIGHT_COUNT];\n"
            "uniform int u_UsedLightSources;\n"
            "uniform samplerCube u_ShadowMap;\n"
            "uniform float u_FarPlane;\n"
            "uniform vec3 u_LightPosition;\n"
            "uniform vec3 u_CameraPosition;\n"
            "\n"
            "in vec3 f_Position;\n"
            "in vec3 f_Normal;\n"
            "in vec2 f_TexCoord;\n"
            "\n"
            "void main()\n"
            "{\n"
            "#ifdef SHADOW_MAP\n"
            "   float shadow = calculatePointShadow(f_Position, u_ShadowMap, u_FarPlane, u_LightPosition, u_CameraPosition);\n"
            "#else\n"
            "   float shadow = 0.0;\n"
            "#endif\n"
            "   f_FinalColor = texture(u_Texture, f_TexCoord) * calculateLightDiffuse(f_Position, normalize(f_Normal), u_LightSources, u_UsedLightSources, shadow);\n"
            "}\n";

        s_LitTextureShader[0] = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource);
        s_LitTextureShader[1] = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource, ShaderDefines{ "SHADOW_MAP" });
    }

    Ref<Shader> GraphicsCache::CreateDefaultColorAnimatedShader(int maxJoints)
    {
        auto it = s_DefaultColorAnimatedShaders.find(maxJoints);
        if (it != s_DefaultColorAnimatedShaders.end())
            return it->second;
        std::string vertexShaderSource =
            SHADER_VERSION_STRING + '\n' +
            "layout (location = 0) in vec3 v_Position;\n"
            "layout (location = 4) in ivec4 v_JointIds;\n"
            "layout (location = 5) in vec4 v_JointWeights;\n"
            "\n"
            "uniform mat4 u_ModelMatrix;\n"
            "uniform mat4 u_ProjViewMatrix;\n"
            "\n"
            "uniform mat4 u_JointTransforms[JOINT_COUNT];\n"
            "\n"
            "void main()\n"
            "{\n"
            "    vec4 localPosition = vec4(0.0);\n"
            "\n"
            "    for (int i = 0; i < 4; i++)\n"
            "    {\n"
            "        mat4 jointTransform = u_JointTransforms[v_JointIds[i]];\n"
            "        vec4 posePosition = jointTransform * vec4(v_Position, 1.0);\n"
            "        localPosition += posePosition * v_JointWeights[i];\n"
            "    }\n"
            "\n"
            "    gl_Position = u_ProjViewMatrix * u_ModelMatrix * localPosition;\n"
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

        Ref<Shader> shader = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource, ShaderDefines{ "JOINT_COUNT=" + std::to_string(maxJoints) });
        s_DefaultColorAnimatedShaders[maxJoints] = shader;
        return shader;
    }

    Ref<Shader> GraphicsCache::CreateLitTextureAnimatedShader(int maxJoints)
    {
        auto it = s_LitTextureAnimatedShaders.find(maxJoints);
        if (it != s_LitTextureAnimatedShaders.end())
            return it->second;
        std::string vertexShaderSource =
            SHADER_VERSION_STRING + '\n' +
            "layout (location = 0) in vec3 v_Position;\n"
            "layout (location = 1) in vec3 v_Normal;\n"
            "layout (location = 2) in vec2 v_TexCoord;\n"
            "layout (location = 4) in ivec4 v_JointIds;\n"
            "layout (location = 5) in vec4 v_JointWeights;\n"
            "\n"
            "uniform mat4 u_ModelMatrix;\n"
            "uniform mat4 u_ProjViewMatrix;\n"
            "\n"
            "uniform mat4 u_JointTransforms[JOINT_COUNT];\n"
            "\n"
            "out vec3 f_Position;\n"
            "out vec3 f_Normal;\n"
            "out vec2 f_TexCoord;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    vec4 localPosition = vec4(0.0);\n"
            "    vec4 normal = vec4(0.0);\n"
            "\n"
            "    for (int i = 0; i < 4; i++)\n"
            "    {\n"
            "        mat4 jointTransform = u_JointTransforms[v_JointIds[i]];\n"
            "        vec4 posePosition = jointTransform * vec4(v_Position, 1.0);\n"
            "        localPosition += posePosition * v_JointWeights[i];\n"
            "\n"
            "        vec4 worldNormal = jointTransform * vec4(v_Normal, 0.0);\n"
            "        normal += worldNormal * v_JointWeights[i];\n"
            "    }\n"
            "\n"
            "    gl_Position = u_ProjViewMatrix * u_ModelMatrix * localPosition;\n"
            "    f_Position = vec3(u_ModelMatrix * localPosition);\n"
            "    f_Normal = normalize(normal.xyz);\n"
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
            "    float shadow = 0.0;\n"
            "    f_FinalColor = texture(u_Texture, f_TexCoord) * calculateLightDiffuse(f_Position, normalize(f_Normal), u_LightSources, u_UsedLightSources, shadow);\n"
            "}\n";

        Ref<Shader> shader = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource, ShaderDefines{ "JOINT_COUNT=" + std::to_string(maxJoints) });
        s_LitTextureAnimatedShaders[maxJoints] = shader;
        return shader;
    }

    void GraphicsCache::CreateDefaultShadowShader()
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
            "void main()\n"
            "{\n"
            "}\n";

        s_DefaultShadowShader = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource);
    }

    void GraphicsCache::CreateDefaultPointShadowShader()
    {
        std::string vertexShaderSource =
            SHADER_VERSION_STRING + '\n' +
            "layout (location = 0) in vec3 v_Position;\n"
            "\n"
            "uniform mat4 u_ModelMatrix;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    gl_Position = u_ModelMatrix * vec4(v_Position, 1.0);\n"
            "}\n";

        std::string geometryShaderSource =
            SHADER_VERSION_STRING + '\n' +
            "layout(triangles) in;\n"
            "layout(triangle_strip, max_vertices=18) out;\n"
            "\n"
            "uniform mat4 u_PointShadowMatrices[6];\n"
            "out vec4 f_FragPosition;\n"
            "void main()\n"
            "{\n"
            "   for (int face = 0; face < 6; face++)\n"
            "   {\n"
            "       gl_Layer = face;\n"
            "       for (int i = 0; i < 3; i++)\n"
            "       {\n"
            "           f_FragPosition = gl_in[i].gl_Position;\n"
            "           gl_Position = u_PointShadowMatrices[face] * f_FragPosition;\n"
            "           EmitVertex();\n"
            "       }\n"
            "       EndPrimitive();\n"
            "   }\n"
            "}\n";

        std::string fragmentShaderSource =
            SHADER_VERSION_STRING + '\n' +
            "in vec4 f_FragPosition;\n"
            "uniform vec3 u_LightPosition;\n"
            "uniform float u_FarPlane;\n"
            "void main()\n"
            "{\n"
            "   float lightDistance = length(f_FragPosition.xyz - u_LightPosition);\n"
            "   lightDistance = lightDistance / u_FarPlane;\n"
            "   gl_FragDepth = lightDistance;\n"
            "}\n";

        s_DefaultPointShadowShader = Shader::CreateFromSource(vertexShaderSource, geometryShaderSource, fragmentShaderSource);
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

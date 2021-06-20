#pragma warning(disable : 26444)
#pragma warning(disable : 26812)

#include "ForgePch.h"
#include "GraphicsCache.h"
#include "Renderer/Layout.h"
#include "Utils/Readers/ObjReader.h"

#include "Math/Constants.h"

namespace Forge
{

    const AssetLocation DefaultColorShaderAssetLocation = { "DefaultColor", AssetLocationSource::Generated, AssetFlags_None, AssetLocationType::Shader };
    const AssetLocation DefaultTextureShaderAssetLocation = { "DefaultTexture", AssetLocationSource::Generated, AssetFlags_None, AssetLocationType::Shader };
    const AssetLocation LitColorNoShadowShaderAssetLocation = { "LitColor", AssetLocationSource::Generated, AssetFlags_None, AssetLocationType::Shader };
    const AssetLocation LitColorShaderAssetLocation = { "LitColor", AssetLocationSource::Generated, AssetFlags_ShaderShadows, AssetLocationType::Shader };
    const AssetLocation LitTextureNoShadowShaderAssetLocation = { "LitTexture", AssetLocationSource::Generated, AssetFlags_None, AssetLocationType::Shader };
    const AssetLocation LitTextureShaderAssetLocation = { "LitTexture", AssetLocationSource::Generated, AssetFlags_ShaderShadows, AssetLocationType::Shader };
    const AssetLocation DefaultShadowShaderAssetLocation = { "DefaultShadow", AssetLocationSource::Generated, AssetFlags_None, AssetLocationType::Shader };
    const AssetLocation DefaultPointShadowShaderAssetLocation = { "DefaultPointShadow", AssetLocationSource::Generated, AssetFlags_None, AssetLocationType::Shader };
    const AssetLocation DefaultPickShaderAssetLocation = { "DefaultPick", AssetLocationSource::Generated, AssetFlags_None, AssetLocationType::Shader };

    const AssetLocation SquareMeshAssetLocation = { "Square", AssetLocationSource::Generated, AssetFlags_None, AssetLocationType::Mesh };
    const AssetLocation CubeMeshAssetLocation = { "Cube", AssetLocationSource::Generated, AssetFlags_None, AssetLocationType::Mesh };
    const AssetLocation SphereMeshAssetLocation = { "Sphere", AssetLocationSource::Generated, AssetFlags_None, AssetLocationType::Mesh };

    const AssetLocation WhiteTextureAssetLocation = { "White", AssetLocationSource::Generated, AssetFlags_None, AssetLocationType::Texture2D };

    AssetLocation GetGridMeshAssetLocation(int xVertices, int zVertices)
    {
        return { "Grid" + std::to_string(xVertices) + "x" + std::to_string(zVertices), AssetLocationSource::Generated, AssetFlags_None, AssetLocationType::Mesh };
    }

    static std::string SHADER_VERSION_STRING = "#version 450 core";

    Ref<Shader> GraphicsCache::s_DefaultColorShader;
    Ref<Shader> GraphicsCache::s_DefaultTextureShader;
    Ref<Shader> GraphicsCache::s_LitColorShader[2];
    Ref<Shader> GraphicsCache::s_LitTextureShader[2];
    Ref<Shader> GraphicsCache::s_DefaultShadowShader;
    Ref<Shader> GraphicsCache::s_DefaultPointShadowShader;
    Ref<Shader> GraphicsCache::s_DefaultPickShader;
    std::unordered_map<int, Ref<Shader>> GraphicsCache::s_DefaultColorAnimatedShaders;
    std::unordered_map<int, Ref<Shader>> GraphicsCache::s_LitTextureAnimatedShaders;

    Ref<Mesh> GraphicsCache::s_SquareMesh;
    Ref<Mesh> GraphicsCache::s_CubeMesh;
    Ref<Mesh> GraphicsCache::s_SphereMesh;

    Ref<Texture2D> GraphicsCache::s_WhiteTexture;

    std::unordered_map<AssetLocation, std::weak_ptr<Shader>> GraphicsCache::s_Shaders;
    std::unordered_map<AssetLocation, std::weak_ptr<Mesh>> GraphicsCache::s_Meshes;
    std::unordered_map<AssetLocation, std::weak_ptr<Texture2D>> GraphicsCache::s_Texture2Ds;
    std::unordered_map<AssetLocation, std::weak_ptr<TextureCube>> GraphicsCache::s_TextureCubes;
    std::unordered_map<void*, AssetLocation> GraphicsCache::s_AssetLocations;

    void GraphicsCache::Init()
    {
    }

    Ref<Texture2D> GraphicsCache::LoadTexture2D(const std::string& filename, AssetFlags flags)
    {
        auto it = s_Texture2Ds.find({ filename, AssetLocationSource::File, flags, AssetLocationType::Texture2D });
        if (it != s_Texture2Ds.end() && !it->second.expired())
            return it->second.lock();
        Ref<Texture2D> texture = Texture2D::Create(filename);
        if (texture)
        {
            RegisterNewAsset({ filename, AssetLocationSource::File, flags }, texture, s_Texture2Ds);
        }
        FORGE_INFO("Loaded Asset: {}", filename);
        return texture;
    }

    Ref<TextureCube> GraphicsCache::LoadTextureCube(const std::string& front, const std::string& back, const std::string& left, const std::string& right, const std::string& bottom, const std::string& top, AssetFlags flags)
    {
        auto it = s_TextureCubes.find({ front, AssetLocationSource::File, flags , AssetLocationType::TextureCube });
        if (it != s_TextureCubes.end() && !it->second.expired())
            return it->second.lock();
        Ref<TextureCube> texture = TextureCube::Create(
            front,
            back,
            left,
            right,
            bottom,
            top
        );
        if (texture)
        {
            RegisterNewAsset({ front, AssetLocationSource::File, flags }, texture, s_TextureCubes);
        }
        FORGE_INFO("Loaded Asset: {}", front);
        return texture;
    }

    Ref<Mesh> GraphicsCache::LoadMesh(const std::string& filename, AssetFlags flags)
    {
        auto it = s_Meshes.find({ filename, AssetLocationSource::File, flags, AssetLocationType::Mesh });
        if (it != s_Meshes.end() && !it->second.expired())
            return it->second.lock();
        ObjReader reader(filename);
        if (reader.GetMesh())
        {
            RegisterNewAsset({ filename, AssetLocationSource::File, flags }, reader.GetMesh(), s_Meshes);
        }
        FORGE_INFO("Loaded Asset: {}", filename);
        return reader.GetMesh();
    }

    Ref<Shader> GraphicsCache::LoadShader(const std::string& filename, AssetFlags flags)
    {
        auto it = s_Shaders.find({ filename, AssetLocationSource::File, flags, AssetLocationType::Shader });
        if (it != s_Shaders.end() && !it->second.expired())
            return it->second.lock();
        ShaderDefines defines;
        if (flags & AssetFlags_ShaderShadows)
            defines.push_back(ShadowMapShaderDefine);
        Ref<Shader> shader = Shader::CreateFromFile(filename, defines);
        if (shader)
        {
            RegisterNewAsset({ filename, AssetLocationSource::File, flags }, shader, s_Shaders);
        }
        FORGE_INFO("Loaded Asset: {}", filename);
        return shader;
    }

    Ref<Material> GraphicsCache::DefaultColorMaterial(const Color& color)
    {
        Ref<Material> material = CreateRef<Material>(DefaultColorShader());
        material->GetUniforms().SetUniform("u_Color", color);
        return material;
    }

    Ref<Material> GraphicsCache::DefaultTextureMaterial(const Ref<Texture>& texture)
    {
        Ref<Material> material = CreateRef<Material>(DefaultTextureShader());
        material->GetUniforms().SetUniform("u_Texture", texture);
        return material;
    }

    Ref<Material> GraphicsCache::LitColorMaterial(const Color& color)
    {
        Ref<Material> material = CreateRef<Material>(MaterialShaderSet{ DefaultPointShadowShader(), LitColorShader(true), LitColorShader(false), DefaultPickShader() });
        material->GetUniforms().SetUniform("u_Color", color);
        return material;
    }

    Ref<Material> GraphicsCache::LitTextureMaterial(const Ref<Texture>& texture)
    {
        Ref<Material> material = CreateRef<Material>(MaterialShaderSet{ DefaultPointShadowShader(), LitTextureShader(true), LitTextureShader(false), DefaultPickShader() });
        material->GetUniforms().SetUniform("u_Texture", texture);
        return material;
    }

    Ref<Material> GraphicsCache::AnimatedDefaultColorMaterial(int maxJoints, const Color& color)
    {
        Ref<Material> material = CreateRef<Material>(AnimatedDefaultColorShader(maxJoints));
        material->GetUniforms().SetUniform("u_Color", color);
        return material;
    }

    Ref<Material> GraphicsCache::AnimatedLitTextureMaterial(int maxJoints, const Ref<Texture>& texture)
    {
        Ref<Material> material = CreateRef<Material>(AnimatedLitTextureShader(maxJoints));
        material->GetUniforms().SetUniform("u_Texture", texture);
        return material;
    }

    void GraphicsCache::CreateDefaultColorShader()
    {
        if (!s_DefaultColorShader)
        {
            std::string vertexShaderSource =
                SHADER_VERSION_STRING + '\n' +
                "#include \"Clipping.h\"\n"
                "layout (location = 0) in vec3 v_Position;\n"
                "\n"
                "uniform mat4 frg_ModelMatrix;\n"
                "uniform mat4 frg_ProjViewMatrix;\n"
                "uniform vec4 frg_ClippingPlanes[MAX_CLIPPING_PLANES];\n"
                "uniform int frg_UsedClippingPlanes;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    vec4 worldPosition = frg_ModelMatrix * vec4(v_Position, 1.0);\n"
                "    clipPlanes(worldPosition.xyz, frg_ClippingPlanes, frg_UsedClippingPlanes);\n"
                "    gl_Position = frg_ProjViewMatrix * worldPosition;\n"
                "}\n";

            std::string fragmentShaderSource =
                SHADER_VERSION_STRING + '\n' +
                "layout (location = 0) out vec4 f_FinalColor;\n"
                "\n"
                "[\"Color\"]\n"
                "uniform vec4 u_Color;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    f_FinalColor = u_Color;\n"
                "}\n";

            s_DefaultColorShader = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource);
            RegisterNewAsset(DefaultColorShaderAssetLocation, s_DefaultColorShader, s_Shaders);
        }
    }

    void GraphicsCache::CreateDefaultTextureShader()
    {
        if (!s_DefaultTextureShader)
        {
            std::string vertexShaderSource =
                SHADER_VERSION_STRING + '\n' +
                "layout (location = 0) in vec3 v_Position;\n"
                "layout (location = 2) in vec2 v_TexCoord;\n"
                "\n"
                "uniform mat4 frg_ModelMatrix;\n"
                "uniform mat4 frg_ProjViewMatrix;\n"
                "\n"
                "out vec2 f_TexCoord;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    gl_Position = frg_ProjViewMatrix * frg_ModelMatrix * vec4(v_Position, 1.0);\n"
                "    f_TexCoord = v_TexCoord;\n"
                "}\n";

            std::string fragmentShaderSource =
                SHADER_VERSION_STRING + '\n' +
                "layout (location = 0) out vec4 f_FinalColor;\n"
                "\n"
                "[\"Texture\"]\n"
                "uniform sampler2D u_Texture;\n"
                "\n"
                "in vec2 f_TexCoord;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    f_FinalColor = texture(u_Texture, f_TexCoord);\n"
                "}\n";

            s_DefaultTextureShader = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource);
            RegisterNewAsset(DefaultTextureShaderAssetLocation, s_DefaultTextureShader, s_Shaders);
        }
    }

    void GraphicsCache::CreateLitColorShader()
    {
        if (!s_LitColorShader[0])
        {
            std::string vertexShaderSource =
                SHADER_VERSION_STRING + '\n' +
                "#include \"Clipping.h\"\n"
                "layout (location = 0) in vec3 v_Position;\n"
                "layout (location = 1) in vec3 v_Normal;\n"
                "\n"
                "uniform mat4 frg_ModelMatrix;\n"
                "uniform mat4 frg_ProjViewMatrix;\n"
                "uniform vec4 frg_ClippingPlanes[MAX_CLIPPING_PLANES];\n"
                "uniform int frg_UsedClippingPlanes;\n"
                "\n"
                "out vec3 f_Position;\n"
                "out vec3 f_Normal;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    vec4 worldPosition = frg_ModelMatrix * vec4(v_Position, 1.0);\n"
                "    clipPlanes(worldPosition.xyz, frg_ClippingPlanes, frg_UsedClippingPlanes);\n"
                "    gl_Position = frg_ProjViewMatrix * worldPosition;\n"
                "    f_Position = worldPosition.xyz;\n"
                "    f_Normal = vec3(transpose(inverse(frg_ModelMatrix)) * vec4(v_Normal, 0.0));\n"
                "}\n";

            std::string fragmentShaderSource =
                SHADER_VERSION_STRING + '\n' +
                "#include <Lighting.h>\n"
                "#include <Shadows.h>\n"
                "\n"
                "layout (location = 0) out vec4 f_FinalColor;\n"
                "\n"
                "[\"Color\"]\n"
                "uniform vec4 u_Color;\n"
                "uniform LightSource frg_LightSources[MAX_LIGHT_COUNT];\n"
                "uniform int frg_UsedLightSources;\n"
                "uniform samplerCube frg_ShadowMap;\n"
                "uniform float frg_FarPlane;\n"
                "uniform vec3 frg_LightPosition;\n"
                "uniform vec3 frg_CameraPosition;\n"
                "\n"
                "in vec3 f_Position;\n"
                "in vec3 f_Normal;\n"
                "\n"
                "void main()\n"
                "{\n"
                "#ifdef SHADOW_MAP\n"
                "    float shadow = calculatePointShadow(f_Position, frg_ShadowMap, frg_FarPlane, frg_LightPosition, frg_CameraPosition);\n"
                "#else\n"
                "    float shadow = 0.0;\n"
                "#endif\n"
                "    f_FinalColor = u_Color * calculateLightDiffuse(f_Position, normalize(f_Normal), frg_LightSources, frg_UsedLightSources, shadow);\n"
                "}\n";

            s_LitColorShader[0] = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource);
            s_LitColorShader[1] = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource, ShaderDefines{ ShadowMapShaderDefine });
            RegisterNewAsset(LitColorNoShadowShaderAssetLocation, s_LitColorShader[0], s_Shaders);
            RegisterNewAsset(LitColorShaderAssetLocation, s_LitColorShader[1], s_Shaders);
        }
    }

    void GraphicsCache::CreateLitTextureShader()
    {
        if (!s_LitTextureShader[0])
        {
            std::string vertexShaderSource =
                SHADER_VERSION_STRING + '\n' +
                "layout (location = 0) in vec3 v_Position;\n"
                "layout (location = 1) in vec3 v_Normal;\n"
                "layout (location = 2) in vec2 v_TexCoord;\n"
                "\n"
                "uniform mat4 frg_ModelMatrix;\n"
                "uniform mat4 frg_ProjViewMatrix;\n"
                "\n"
                "out vec3 f_Position;\n"
                "out vec3 f_Normal;\n"
                "out vec2 f_TexCoord;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    gl_Position = frg_ProjViewMatrix * frg_ModelMatrix * vec4(v_Position, 1.0);\n"
                "    f_Position = vec3(frg_ModelMatrix * vec4(v_Position, 1.0));\n"
                "    f_Normal = vec3(transpose(inverse(frg_ModelMatrix)) * vec4(v_Normal, 0.0));\n"
                "    f_TexCoord = v_TexCoord;\n"
                "}\n";

            std::string fragmentShaderSource =
                SHADER_VERSION_STRING + '\n' +
                "#include <Lighting.h>\n"
                "#include <Shadows.h>\n"
                "\n"
                "layout (location = 0) out vec4 f_FinalColor;\n"
                "\n"
                "[\"Texture\"]\n"
                "uniform sampler2D u_Texture;\n"
                "uniform LightSource frg_LightSources[MAX_LIGHT_COUNT];\n"
                "uniform int frg_UsedLightSources;\n"
                "uniform samplerCube frg_ShadowMap;\n"
                "uniform float frg_FarPlane;\n"
                "uniform vec3 frg_LightPosition;\n"
                "uniform vec3 frg_CameraPosition;\n"
                "\n"
                "in vec3 f_Position;\n"
                "in vec3 f_Normal;\n"
                "in vec2 f_TexCoord;\n"
                "\n"
                "void main()\n"
                "{\n"
                "#ifdef SHADOW_MAP\n"
                "   float shadow = calculatePointShadow(f_Position, frg_ShadowMap, frg_FarPlane, frg_LightPosition, frg_CameraPosition);\n"
                "#else\n"
                "   float shadow = 0.0;\n"
                "#endif\n"
                "   f_FinalColor = texture(u_Texture, f_TexCoord) * calculateLightDiffuse(f_Position, normalize(f_Normal), frg_LightSources, frg_UsedLightSources, shadow);\n"
                "}\n";

            s_LitTextureShader[0] = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource);
            s_LitTextureShader[1] = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource, ShaderDefines{ ShadowMapShaderDefine });
            RegisterNewAsset(LitTextureNoShadowShaderAssetLocation, s_LitTextureShader[0], s_Shaders);
            RegisterNewAsset(LitTextureShaderAssetLocation, s_LitTextureShader[1], s_Shaders);
        }
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
            "uniform mat4 frg_ModelMatrix;\n"
            "uniform mat4 frg_ProjViewMatrix;\n"
            "\n"
            "uniform mat4 frg_JointTransforms[JOINT_COUNT];\n"
            "\n"
            "void main()\n"
            "{\n"
            "    vec4 localPosition = vec4(0.0);\n"
            "\n"
            "    for (int i = 0; i < 4; i++)\n"
            "    {\n"
            "        mat4 jointTransform = frg_JointTransforms[v_JointIds[i]];\n"
            "        vec4 posePosition = jointTransform * vec4(v_Position, 1.0);\n"
            "        localPosition += posePosition * v_JointWeights[i];\n"
            "    }\n"
            "\n"
            "    gl_Position = frg_ProjViewMatrix * frg_ModelMatrix * localPosition;\n"
            "}\n";

        std::string fragmentShaderSource =
            SHADER_VERSION_STRING + '\n' +
            "layout (location = 0) out vec4 f_FinalColor;\n"
            "\n"
            "[\"Color\"]\n"
            "uniform vec4 u_Color;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    f_FinalColor = u_Color;\n"
            "}\n";

        Ref<Shader> shader = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource, ShaderDefines{ "JOINT_COUNT=" + std::to_string(maxJoints) });
        s_DefaultColorAnimatedShaders[maxJoints] = shader;
        RegisterNewAsset({ "ColorAnimated" + std::to_string(maxJoints), AssetLocationSource::Generated }, shader, s_Shaders);
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
            "uniform mat4 frg_ModelMatrix;\n"
            "uniform mat4 frg_ProjViewMatrix;\n"
            "\n"
            "uniform mat4 frg_JointTransforms[JOINT_COUNT];\n"
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
            "        mat4 jointTransform = frg_JointTransforms[v_JointIds[i]];\n"
            "        vec4 posePosition = jointTransform * vec4(v_Position, 1.0);\n"
            "        localPosition += posePosition * v_JointWeights[i];\n"
            "\n"
            "        vec4 worldNormal = jointTransform * vec4(v_Normal, 0.0);\n"
            "        normal += worldNormal * v_JointWeights[i];\n"
            "    }\n"
            "\n"
            "    gl_Position = frg_ProjViewMatrix * frg_ModelMatrix * localPosition;\n"
            "    f_Position = vec3(frg_ModelMatrix * localPosition);\n"
            "    f_Normal = normalize(normal.xyz);\n"
            "    f_TexCoord = v_TexCoord;\n"
            "}\n";

        std::string fragmentShaderSource =
            SHADER_VERSION_STRING + '\n' +
            "#include <Lighting.h>\n"
            "\n"
            "layout (location = 0) out vec4 f_FinalColor;\n"
            "\n"
            "[\"Texture\"]\n"
            "uniform sampler2D u_Texture;\n"
            "uniform LightSource frg_LightSources[MAX_LIGHT_COUNT];\n"
            "uniform int frg_UsedLightSources;\n"
            "\n"
            "in vec3 f_Position;\n"
            "in vec3 f_Normal;\n"
            "in vec2 f_TexCoord;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    float shadow = 0.0;\n"
            "    f_FinalColor = texture(u_Texture, f_TexCoord) * calculateLightDiffuse(f_Position, normalize(f_Normal), frg_LightSources, frg_UsedLightSources, shadow);\n"
            "}\n";

        Ref<Shader> shader = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource, ShaderDefines{ "JOINT_COUNT=" + std::to_string(maxJoints) });
        s_LitTextureAnimatedShaders[maxJoints] = shader;
        RegisterNewAsset({ "LitTextureAnimated" + std::to_string(maxJoints), AssetLocationSource::Generated }, shader, s_Shaders);
        return shader;
    }

    void GraphicsCache::CreateDefaultShadowShader()
    {
        if (!s_DefaultShadowShader)
        {
            std::string vertexShaderSource =
                SHADER_VERSION_STRING + '\n' +
                "layout (location = 0) in vec3 v_Position;\n"
                "\n"
                "uniform mat4 frg_ModelMatrix;\n"
                "uniform mat4 frg_ProjViewMatrix;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    gl_Position = frg_ProjViewMatrix * frg_ModelMatrix * vec4(v_Position, 1.0);\n"
                "}\n";

            std::string fragmentShaderSource =
                SHADER_VERSION_STRING + '\n' +
                "void main()\n"
                "{\n"
                "}\n";

            s_DefaultShadowShader = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource);
            RegisterNewAsset(DefaultShadowShaderAssetLocation, s_DefaultShadowShader, s_Shaders);
        }
    }

    void GraphicsCache::CreateDefaultPointShadowShader()
    {
        if (!s_DefaultPointShadowShader)
        {
            std::string vertexShaderSource =
                SHADER_VERSION_STRING + '\n' +
                "layout (location = 0) in vec3 v_Position;\n"
                "\n"
                "uniform mat4 frg_ModelMatrix;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    gl_Position = frg_ModelMatrix * vec4(v_Position, 1.0);\n"
                "}\n";

            std::string geometryShaderSource =
                SHADER_VERSION_STRING + '\n' +
                "layout(triangles) in;\n"
                "layout(triangle_strip, max_vertices=18) out;\n"
                "\n"
                "uniform mat4 frg_PointShadowMatrices[6];\n"
                "out vec4 f_FragPosition;\n"
                "void main()\n"
                "{\n"
                "   for (int face = 0; face < 6; face++)\n"
                "   {\n"
                "       gl_Layer = face;\n"
                "       for (int i = 0; i < 3; i++)\n"
                "       {\n"
                "           f_FragPosition = gl_in[i].gl_Position;\n"
                "           gl_Position = frg_PointShadowMatrices[face] * f_FragPosition;\n"
                "           EmitVertex();\n"
                "       }\n"
                "       EndPrimitive();\n"
                "   }\n"
                "}\n";

            std::string fragmentShaderSource =
                SHADER_VERSION_STRING + '\n' +
                "in vec4 f_FragPosition;\n"
                "uniform vec3 frg_ShadowLightPosition;\n"
                "uniform float frg_FarPlane;\n"
                "void main()\n"
                "{\n"
                "   float lightDistance = length(f_FragPosition.xyz - frg_ShadowLightPosition);\n"
                "   lightDistance = lightDistance / frg_FarPlane;\n"
                "   gl_FragDepth = lightDistance;\n"
                "}\n";

            s_DefaultPointShadowShader = Shader::CreateFromSource(vertexShaderSource, geometryShaderSource, fragmentShaderSource);
            RegisterNewAsset(DefaultPointShadowShaderAssetLocation, s_DefaultPointShadowShader, s_Shaders);
        }
    }

    void GraphicsCache::CreateDefaultPickShader()
    {
        if (!s_DefaultPickShader)
        {
            std::string vertexShaderSource =
                SHADER_VERSION_STRING + '\n' +
                "layout (location = 0) in vec3 v_Position;\n"
                "\n"
                "uniform mat4 frg_ModelMatrix;\n"
                "uniform mat4 frg_ProjViewMatrix;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    gl_Position = frg_ProjViewMatrix * frg_ModelMatrix * vec4(v_Position, 1.0);\n"
                "}\n";

            std::string fragmentShaderSource =
                SHADER_VERSION_STRING + '\n' +
                "layout (location = 0) out int f_EntityID;\n"
                "uniform int frg_EntityID;\n"
                "void main()\n"
                "{\n"
                "   f_EntityID = frg_EntityID;\n"
                "}\n";

            s_DefaultPickShader = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource);
            RegisterNewAsset(DefaultPickShaderAssetLocation, s_DefaultPickShader, s_Shaders);
        }
    }

    void GraphicsCache::CreateSquareMesh()
    {
        if (!s_SquareMesh)
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
            RegisterNewAsset(SquareMeshAssetLocation, s_SquareMesh, s_Meshes);
        }
    }

    void GraphicsCache::CreateCubeMesh()
    {
        if (!s_CubeMesh)
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
            RegisterNewAsset(CubeMeshAssetLocation, s_CubeMesh, s_Meshes);
        }
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

        Ref<Mesh> mesh = CreateRef<Mesh>(vao);
        RegisterNewAsset(GetGridMeshAssetLocation(xVertices, zVertices), mesh, s_Meshes);
        return mesh;
    }

    void GraphicsCache::CreateSphereMesh()
    {
        if (!s_SphereMesh)
        {
            float radius = 1.0f;
            size_t sectorCount = 90;

            size_t indexCount = 6 * sectorCount * (sectorCount - 1);
            size_t vertexCount = (sectorCount + 1) * (sectorCount + 1);

            BufferLayout layout({
                { ShaderDataType::Float3 },
                { ShaderDataType::Float3 },
                { ShaderDataType::Float2 },
            });

            float* vertices = new float[vertexCount * 8];
            uint32_t* indices = new uint32_t[indexCount];

            float x, y, z, xy;                              // vertex position
            float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
            float s, t;                                     // vertex texCoord

            float sectorStep = 2 * PI / sectorCount;
            float stackStep = PI / sectorCount;
            float sectorAngle, stackAngle;

            size_t index = 0;
            for (int i = 0; i <= sectorCount; i++)
            {
                stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
                xy = radius * cosf(stackAngle);             // r * cos(u)
                z = radius * sinf(stackAngle);              // r * sin(u)

                // add (sectorCount+1) vertices per stack
                // the first and last vertices have same position and normal, but different tex coords
                for (int j = 0; j <= sectorCount; j++)
                {
                    sectorAngle = j * sectorStep;           // starting from 0 to 2pi

                    // vertex position (x, y, z)
                    x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
                    y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
                    vertices[index++] = x;
                    vertices[index++] = y;
                    vertices[index++] = z;

                    // normalized vertex normal (nx, ny, nz)
                    nx = x * lengthInv;
                    ny = y * lengthInv;
                    nz = z * lengthInv;
                    vertices[index++] = nx;
                    vertices[index++] = ny;
                    vertices[index++] = nz;

                    // vertex tex coord (s, t) range between [0, 1]
                    s = (float)j / sectorCount;
                    t = (float)i / sectorCount;
                    vertices[index++] = s;
                    vertices[index++] = t;
                }
            }

            index = 0;
            int k1, k2;
            for (int i = 0; i < sectorCount; ++i)
            {
                k1 = i * (sectorCount + 1);     // beginning of current stack
                k2 = k1 + sectorCount + 1;      // beginning of next stack

                for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
                {
                    // 2 triangles per sector excluding first and last stacks
                    // k1 => k2 => k1+1
                    if (i != 0)
                    {
                        indices[index++] = (k1);
                        indices[index++] = (k2);
                        indices[index++] = (k1 + 1);
                    }

                    // k1+1 => k2 => k2+1
                    if (i != (sectorCount - 1))
                    {
                        indices[index++] = (k1 + 1);
                        indices[index++] = (k2);
                        indices[index++] = (k2 + 1);
                    }
                }
            }
            
            Ref<VertexBuffer> vbo = VertexBuffer::Create(vertices, vertexCount * layout.GetStride(), layout);
            Ref<IndexBuffer> ibo = IndexBuffer::Create(indices, indexCount * sizeof(uint32_t));
            Ref<VertexArray> vao = VertexArray::Create();
            vao->AddVertexBuffer(vbo);
            vao->SetIndexBuffer(ibo);

            delete[] vertices;
            delete[] indices;

            s_SphereMesh = CreateRef<Mesh>(vao);
            RegisterNewAsset(SphereMeshAssetLocation, s_SphereMesh, s_Meshes);
        }
    }

    void GraphicsCache::CreateWhiteTexture()
    {
        if (!s_WhiteTexture)
        {
            uint8_t pixels[4]{ 255, 255, 255, 255 };
            Ref<Texture2D> texture = Texture2D::Create(1, 1, pixels);
            s_WhiteTexture = texture;
            RegisterNewAsset(WhiteTextureAssetLocation, s_WhiteTexture, s_Texture2Ds);
        }
    }

    void GraphicsCache::HandleGeneratedShader(const AssetLocation& location)
    {
        if (s_Shaders.find(location) == s_Shaders.end())
        {
            if (location.Path == DefaultColorShaderAssetLocation.Path)
            {
                CreateDefaultColorShader();
            }
            else if (location.Path == DefaultTextureShaderAssetLocation.Path)
            {
                CreateDefaultTextureShader();
            }
            else if (location.Path == LitColorShaderAssetLocation.Path)
            {
                CreateLitColorShader();
            }
            else if (location.Path == LitTextureShaderAssetLocation.Path)
            {
                CreateLitTextureShader();
            }
            else if (location.Path == DefaultShadowShaderAssetLocation.Path)
            {
                CreateDefaultShadowShader();
            }
            else if (location.Path == DefaultPointShadowShaderAssetLocation.Path)
            {
                CreateDefaultPointShadowShader();
            }
            else if (location.Path == DefaultPickShaderAssetLocation.Path)
            {
                CreateDefaultPickShader();
            }
        }
    }

    void GraphicsCache::HandleGeneratedTexture2D(const AssetLocation& location)
    {
        if (s_Texture2Ds.find(location) == s_Texture2Ds.end())
        {
            if (location.Path == WhiteTextureAssetLocation.Path)
            {
                CreateWhiteTexture();
            }
        }
    }

    Ref<Mesh> GraphicsCache::HandleGeneratedMesh(const AssetLocation& location)
    {
        if (s_Meshes.find(location) == s_Meshes.end())
        {
            if (location.Path == SquareMeshAssetLocation.Path)
            {
                CreateSquareMesh();
            }
            else if (location.Path == CubeMeshAssetLocation.Path)
            {
                CreateCubeMesh();
            }
            else if (location.Path == SphereMeshAssetLocation.Path)
            {
                CreateSphereMesh();
            }
            else if (location.Path.substr(0, 4) == "Grid")
            {
                size_t x = location.Path.find('x');
                FORGE_ASSERT(x != std::string::npos && x > 4, "Invalid asset path");
                std::string xVerticesString = location.Path.substr(4, x - 4);
                std::string zVerticesString = location.Path.substr(x + 1);
                int xVertices = std::stoi(xVerticesString);
                int zVertices = std::stoi(zVerticesString);
                return GridMesh(xVertices, zVertices);
            }
        }
        return nullptr;
    }

}

#include "ForgePch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"
#include "CameraComponent.h"
#include "ModelRenderer.h"

#include "Assets/GraphicsCache.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace YAML
{

    template<>
    struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::quat>
    {
        static Node encode(const glm::quat& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::quat& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template<>
    struct convert<Forge::Color>
    {
        static Node encode(const Forge::Color& rhs)
        {
            Node node;
            node.push_back(rhs.r);
            node.push_back(rhs.g);
            node.push_back(rhs.b);
            node.push_back(rhs.a);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, Forge::Color& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.r = node[0].as<float>();
            rhs.g = node[1].as<float>();
            rhs.b = node[2].as<float>();
            rhs.a = node[3].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::mat4>
    {
        static Node encode(const glm::mat4& rhs)
        {
            Node node;
            node.push_back(rhs[0]);
            node.push_back(rhs[1]);
            node.push_back(rhs[2]);
            node.push_back(rhs[3]);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::mat4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs[0] = node[0].as<glm::vec4>();
            rhs[1] = node[1].as<glm::vec4>();
            rhs[2] = node[2].as<glm::vec4>();
            rhs[3] = node[3].as<glm::vec4>();
            return true;
        }
    };

    template<>
    struct convert<Forge::Frustum>
    {
        static Node encode(const Forge::Frustum& rhs)
        {
            Node node;
            node["Type"] = int(rhs.Type);
            node["Left"] = rhs.Left;
            node["Right"] = rhs.Right;
            node["Bottom"] = rhs.Bottom;
            node["Top"] = rhs.Top;
            node["Near"] = rhs.NearPlane;
            node["Far"] = rhs.FarPlane;
            return node;
        }

        static bool decode(const Node& node, Forge::Frustum& rhs)
        {
            if (!node.IsMap())
                return false;

            Forge::ProjectionType type = Forge::ProjectionType(node["Type"].as<int>());
            float left = node["Left"].as<float>();
            float right = node["Right"].as<float>();
            float bottom = node["Bottom"].as<float>();
            float top = node["Top"].as<float>();
            float nearPlane = node["Near"].as<float>();
            float farPlane = node["Far"].as<float>();
            if (type == Forge::ProjectionType::Perspective)
            {
                rhs = Forge::Frustum::Perspective(left, right, bottom, top, nearPlane, farPlane);
            }
            else
            {
                rhs = Forge::Frustum::Orthographic(left, right, bottom, top, nearPlane, farPlane);
            }
            return true;
        }
    };

    template<>
    struct convert<Forge::Viewport>
    {
        static Node encode(const Forge::Viewport& rhs)
        {
            Node node;
            node["Left"] = rhs.Left;
            node["Bottom"] = rhs.Bottom;
            node["Width"] = rhs.Width;
            node["Height"] = rhs.Height;
            return node;
        }

        static bool decode(const Node& node, Forge::Viewport& rhs)
        {
            if (!node.IsMap())
                return false;

            rhs.Left = node["Left"].as<float>();
            rhs.Bottom = node["Bottom"].as<float>();
            rhs.Width = node["Width"].as<float>();
            rhs.Height = node["Height"].as<float>();
            return true;
        }
    };

    template<>
    struct convert<Forge::AssetLocation>
    {
        static Node encode(const Forge::AssetLocation& rhs)
        {
            Node node;
            node["Path"] = rhs.Path;
            node["Type"] = int(rhs.Type);
            node["Source"] = int(rhs.Source);
            node["Flags"] = int(rhs.Flags);
            return node;
        }

        static bool decode(const Node& node, Forge::AssetLocation& rhs)
        {
            if (!node.IsMap())
                return false;

            rhs.Path = node["Path"].as<std::string>();
            rhs.Type = Forge::AssetLocationType(node["Type"].as<int>());
            rhs.Source = Forge::AssetLocationSource(node["Source"].as<int>());
            rhs.Flags = Forge::AssetFlags(node["Flags"].as<int>());
            return true;
        }
    };

}

namespace Forge
{

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::quat& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::mat4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq;
        for (int i = 0; i < 4; i++)
            out << v[i];
        out << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const Color& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.r << v.g << v.b << v.a << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const Frustum& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "Type" << YAML::Value << int(v.Type);
        out << YAML::Key << "Left" << YAML::Value << v.Left;
        out << YAML::Key << "Right" << YAML::Value << v.Right;
        out << YAML::Key << "Bottom" << YAML::Value << v.Bottom;
        out << YAML::Key << "Top" << YAML::Value << v.Top;
        out << YAML::Key << "Near" << YAML::Value << v.NearPlane;
        out << YAML::Key << "Far" << YAML::Value << v.FarPlane;
        out << YAML::EndMap;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const Viewport& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "Left" << YAML::Value << v.Left;
        out << YAML::Key << "Bottom" << YAML::Value << v.Bottom;
        out << YAML::Key << "Width" << YAML::Value << v.Width;
        out << YAML::Key << "Height" << YAML::Value << v.Height;
        out << YAML::EndMap;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const AssetLocation& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "Path" << YAML::Value << v.Path;
        out << YAML::Key << "Type" << YAML::Value << int(v.Type);
        out << YAML::Key << "Source" << YAML::Value << int(v.Source);
        out << YAML::Key << "Flags" << YAML::Value << int(v.Flags);
        out << YAML::EndMap;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const UniformContext& uniforms)
    {
        out << YAML::BeginMap;
        for (const UniformSpecification& specification : uniforms.GetUniforms())
        {
            out << YAML::Key << specification.VariableName;
            out << YAML::BeginMap;
            out << YAML::Key << "Type" << YAML::Value << int(specification.Type);
            out << YAML::Key << "Value";
            switch (specification.Type)
            {
                case ShaderDataType::Int:
                    break;
                case ShaderDataType::Float:
                    out << YAML::Value << uniforms.GetUniform<float>(specification.VariableName);
                    break;
                case ShaderDataType::Float2:
                    out << YAML::Value << uniforms.GetUniform<glm::vec2>(specification.VariableName);
                    break;
                case ShaderDataType::Float3:
                    out << YAML::Value << uniforms.GetUniform<glm::vec3>(specification.VariableName);
                    break;
                case ShaderDataType::Float4:
                    out << YAML::Value << uniforms.GetUniform<Color>(specification.VariableName);
                    break;
                case ShaderDataType::Sampler1D:
                case ShaderDataType::Sampler2D:
                case ShaderDataType::Sampler3D:
                case ShaderDataType::SamplerCube:
                {
                    Ref<Texture> texture = uniforms.GetUniform<Ref<Texture>>(specification.VariableName);
                    if (GraphicsCache::HasAssetLocation(texture))
                    {
                        AssetLocation location = GraphicsCache::GetAssetLocation(texture);
                        out << YAML::Value << location;
                    }
                    else
                    {
                        out << YAML::Value << NullAssetLocation;
                    }
                    break;
                }
            }
            out << YAML::EndMap;
        }
        out << YAML::EndMap;
        return out;
    }

    static void SerializeEntity(YAML::Emitter& emitter, Entity entity)
    {
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "Entity" << YAML::Value << std::to_string(uint32_t(entity));
        emitter << YAML::Key << "Enabled" << YAML::Value << entity.Enabled();
        emitter << YAML::Key << "LayerMask" << YAML::Value << entity.GetComponent<LayerId>().Mask;

        if (entity.HasComponent<TagComponent>())
        {
            emitter << YAML::Key << "TagComponent";
            emitter << YAML::BeginMap;

            emitter << YAML::Key << "Tag" << YAML::Value << entity.GetComponent<TagComponent>().Tag;

            emitter << YAML::EndMap;
        }
        if (entity.HasComponent<TransformComponent>())
        {
            emitter << YAML::Key << "TransformComponent";
            emitter << YAML::BeginMap;

            TransformComponent& transform = entity.GetComponent<TransformComponent>();
            emitter << YAML::Key << "Position" << YAML::Value << transform.GetLocalPosition();
            emitter << YAML::Key << "Rotation" << YAML::Value << transform.GetLocalRotation();
            emitter << YAML::Key << "Scale" << YAML::Value << transform.GetLocalScale();

            emitter << YAML::EndMap;
        }
        if (entity.HasComponent<CameraComponent>())
        {
            emitter << YAML::Key << "CameraComponent";
            emitter << YAML::BeginMap;

            CameraComponent& camera = entity.GetComponent<CameraComponent>();
            emitter << YAML::Key << "Frustum" << YAML::Value << camera.Frustum;
            emitter << YAML::Key << "Viewport" << YAML::Value << camera.Viewport;
            emitter << YAML::Key << "ClearColor" << YAML::Value << camera.ClearColor;
            emitter << YAML::Key << "LayerMask" << YAML::Value << camera.LayerMask;
            emitter << YAML::Key << "Mode" << YAML::Value << int(camera.Mode);
            emitter << YAML::Key << "Priority" << YAML::Value << camera.Priority;

            emitter << YAML::EndMap;
        }
        if (entity.HasComponent<PointLightComponent>())
        {
            emitter << YAML::Key << "PointLightComponent";
            emitter << YAML::BeginMap;

            PointLightComponent& light = entity.GetComponent<PointLightComponent>();
            emitter << YAML::Key << "Color" << YAML::Value << light.Color;
            emitter << YAML::Key << "Intensity" << YAML::Value << light.Intensity;
            emitter << YAML::Key << "Ambient" << YAML::Value << light.Ambient;
            emitter << YAML::Key << "Radius" << YAML::Value << light.Radius;
            emitter << YAML::Key << "Cutoff" << YAML::Value << light.Cutoff;
            emitter << YAML::Key << "CastsShadows" << YAML::Value << light.Shadows.Enabled;
            emitter << YAML::Key << "ShadowWidth" << YAML::Value
                    << (light.Shadows.Enabled ? light.Shadows.RenderTarget->GetWidth() : 0);
            emitter << YAML::Key << "ShadowHeight" << YAML::Value
                    << (light.Shadows.Enabled ? light.Shadows.RenderTarget->GetHeight() : 0);

            emitter << YAML::EndMap;
        }
        if (entity.HasComponent<DirectionalLightComponent>())
        {
            emitter << YAML::Key << "DirectionalLightComponent";
            emitter << YAML::BeginMap;

            DirectionalLightComponent& light = entity.GetComponent<DirectionalLightComponent>();
            emitter << YAML::Key << "Color" << YAML::Value << light.Color;
            emitter << YAML::Key << "Intensity" << YAML::Value << light.Intensity;
            emitter << YAML::Key << "Ambient" << YAML::Value << light.Ambient;
            emitter << YAML::Key << "CastsShadows" << YAML::Value << light.Shadows.Enabled;
            emitter << YAML::Key << "ShadowWidth" << YAML::Value
                    << (light.Shadows.Enabled ? light.Shadows.RenderTarget->GetWidth() : 0);
            emitter << YAML::Key << "ShadowHeight" << YAML::Value
                    << (light.Shadows.Enabled ? light.Shadows.RenderTarget->GetHeight() : 0);

            emitter << YAML::EndMap;
        }
        if (entity.HasComponent<ModelRendererComponent>())
        {
            emitter << YAML::Key << "ModelRendererComponent";
            emitter << YAML::BeginMap;

            ModelRendererComponent& renderer = entity.GetComponent<ModelRendererComponent>();
            emitter << YAML::Key << "Model";
            emitter << YAML::BeginSeq;

            for (const Model::SubModel& submodel : renderer.Model->GetSubModels())
            {
                emitter << YAML::BeginMap;

                if (GraphicsCache::HasAssetLocation(submodel.Mesh))
                {
                    AssetLocation location = GraphicsCache::GetAssetLocation(submodel.Mesh);
                    emitter << YAML::Key << "Mesh" << YAML::Value << location;
                }
                else
                {
                    emitter << YAML::Key << "Mesh" << YAML::Value << NullAssetLocation;
                }
                emitter << YAML::Key << "Material";
                emitter << YAML::BeginMap;

                emitter << YAML::Key << "Shaders";
                emitter << YAML::BeginSeq;

                for (int i = 0; i < RENDER_PASS_COUNT; i++)
                {
                    const Ref<Shader>& shader = submodel.Material->GetShader(RenderPass(i));
                    if (GraphicsCache::HasAssetLocation(shader))
                    {
                        AssetLocation location = GraphicsCache::GetAssetLocation(shader);
                        emitter << location;
                    }
                    else
                    {
                        emitter << "NULL";
                    }
                }

                emitter << YAML::EndSeq;
                emitter << YAML::Key << "Uniforms" << YAML::Value << submodel.Material->GetUniforms();

                emitter << YAML::Key << "Settings";
                emitter << YAML::BeginMap;

                emitter << YAML::Key << "PolygonMode" << YAML::Value << int(submodel.Material->GetSettings().Mode);
                emitter << YAML::Key << "CullFace" << YAML::Value << int(submodel.Material->GetSettings().Culling);

                emitter << YAML::EndMap;

                emitter << YAML::EndMap;
                emitter << YAML::Key << "Transform" << YAML::Value << submodel.Transform;

                emitter << YAML::EndMap;
            }

            emitter << YAML::EndSeq;

            emitter << YAML::EndMap;
        }

        emitter << YAML::EndMap;
    }

    static void DeserializeEntity(YAML::Node node, Entity entity)
    {
        YAML::Node transformComponent = node["TransformComponent"];
        if (transformComponent)
        {
            TransformComponent& transform = entity.GetTransform();
            transform.SetLocalPosition(transformComponent["Position"].as<glm::vec3>());
            transform.SetLocalRotation(transformComponent["Rotation"].as<glm::quat>());
            transform.SetLocalScale(transformComponent["Scale"].as<glm::vec3>());
        }
        YAML::Node cameraComponent = node["CameraComponent"];
        if (cameraComponent)
        {
            CameraComponent& cc = entity.AddComponent<CameraComponent>();
            cc.Frustum = cameraComponent["Frustum"].as<Frustum>();
            cc.Viewport = cameraComponent["Viewport"].as<Viewport>();
            cc.ClearColor = cameraComponent["ClearColor"].as<Color>();
            cc.LayerMask = cameraComponent["LayerMask"].as<uint64_t>();
            cc.Mode = CameraMode(cameraComponent["Mode"].as<int>());
            cc.Priority = cameraComponent["Priority"].as<int>();
        }
        YAML::Node pointLightComponent = node["PointLightComponent"];
        if (pointLightComponent)
        {
            PointLightComponent& light = entity.AddComponent<PointLightComponent>();
            light.Color = pointLightComponent["Color"].as<Color>();
            light.Intensity = pointLightComponent["Intensity"].as<float>();
            light.Ambient = pointLightComponent["Ambient"].as<float>();
            light.Radius = pointLightComponent["Radius"].as<float>();
            light.Cutoff = pointLightComponent["Cutoff"].as<float>();
            bool shadowsEnabled = pointLightComponent["CastsShadows"].as<bool>();
            if (shadowsEnabled)
            {
                light.CreateShadowPass(
                  pointLightComponent["ShadowWidth"].as<float>(), pointLightComponent["ShadowHeight"].as<float>());
            }
        }
        YAML::Node directionalLightComponent = node["DirectionalLightComponent"];
        if (directionalLightComponent)
        {
            DirectionalLightComponent& light = entity.AddComponent<DirectionalLightComponent>();
            light.Color = directionalLightComponent["Color"].as<Color>();
            light.Intensity = directionalLightComponent["Intensity"].as<float>();
            light.Ambient = directionalLightComponent["Ambient"].as<float>();
            bool shadowsEnabled = directionalLightComponent["CastsShadows"].as<bool>();
            if (shadowsEnabled)
            {
                light.CreateShadowPass(directionalLightComponent["ShadowWidth"].as<float>(),
                  directionalLightComponent["ShadowHeight"].as<float>());
            }
        }
        YAML::Node modelRendererComponent = node["ModelRendererComponent"];
        if (modelRendererComponent)
        {
            Ref<Model> model = CreateRef<Model>();
            ModelRendererComponent& renderer = entity.AddComponent<ModelRendererComponent>(model);
            YAML::Node modelNode = modelRendererComponent["Model"];

            for (YAML::Node submodelNode : modelNode)
            {
                Model::SubModel submodel;
                AssetLocation meshLocation = submodelNode["Mesh"].as<AssetLocation>();
                submodel.Mesh = GraphicsCache::GetAsset<Mesh>(meshLocation);

                MaterialShaderSet shaders;
                shaders.PickShader = GraphicsCache::GetAsset<Shader>(
                  submodelNode["Material"]["Shaders"][int(RenderPass::Pick)].as<AssetLocation>());
                shaders.WithoutShadowShader = GraphicsCache::GetAsset<Shader>(
                  submodelNode["Material"]["Shaders"][int(RenderPass::WithoutShadow)].as<AssetLocation>());
                shaders.WithShadowShader = GraphicsCache::GetAsset<Shader>(
                  submodelNode["Material"]["Shaders"][int(RenderPass::WithShadow)].as<AssetLocation>());
                shaders.ShadowFormationShaders.PointShadow = GraphicsCache::GetAsset<Shader>(
                  submodelNode["Material"]["Shaders"][int(RenderPass::PointShadowFormation)].as<AssetLocation>());
                shaders.ShadowFormationShaders.Shadow = GraphicsCache::GetAsset<Shader>(
                  submodelNode["Material"]["Shaders"][int(RenderPass::ShadowFormation)].as<AssetLocation>());
                submodel.Material = Material::Create(shaders);

                YAML::Node uniformsNode = submodelNode["Material"]["Uniforms"];
                UniformContext& uniforms = submodel.Material->GetUniforms();
                for (const UniformSpecification& specification : uniforms.GetUniforms())
                {
                    switch (specification.Type)
                    {
                        case ShaderDataType::Int:
                            break;
                        case ShaderDataType::Float:
                            uniforms.SetUniform(specification.VariableName,
                              uniformsNode[specification.VariableName]["Value"].as<float>());
                            break;
                        case ShaderDataType::Float2:
                            uniforms.SetUniform(specification.VariableName,
                              uniformsNode[specification.VariableName]["Value"].as<glm::vec2>());
                            break;
                        case ShaderDataType::Float3:
                            uniforms.SetUniform(specification.VariableName,
                              uniformsNode[specification.VariableName]["Value"].as<glm::vec3>());
                            break;
                        case ShaderDataType::Float4:
                            uniforms.SetUniform(specification.VariableName,
                              uniformsNode[specification.VariableName]["Value"].as<Color>());
                            break;
                        case ShaderDataType::Sampler1D:
                        case ShaderDataType::Sampler2D:
                            uniforms.SetUniform(specification.VariableName,
                              GraphicsCache::GetAsset<Texture2D>(
                                uniformsNode[specification.VariableName]["Value"].as<AssetLocation>()));
                            break;
                        case ShaderDataType::Sampler3D:
                        case ShaderDataType::SamplerCube:
                            uniforms.SetUniform(specification.VariableName,
                              GraphicsCache::GetAsset<TextureCube>(
                                uniformsNode[specification.VariableName]["Value"].as<AssetLocation>()));
                            break;
                    }
                }

                submodel.Material->GetSettings().Culling =
                  CullFace(submodelNode["Material"]["Settings"]["CullFace"].as<int>());
                submodel.Material->GetSettings().Mode =
                  PolygonMode(submodelNode["Material"]["Settings"]["PolygonMode"].as<int>());

                submodel.Transform = submodelNode["Transform"].as<glm::mat4>();
                model->AddSubmodel(submodel);
            }
        }
    }

    SceneSerializer::SceneSerializer(Scene* scene) : m_Scene(scene) {}

    void SceneSerializer::SerializeText(const std::string& filename)
    {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "Scene" << YAML::Value << "Untitled";
        emitter << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        m_Scene->GetRegistry().each(
          [&](entt::entity entity)
          {
              Entity e(entity, &m_Scene->GetRegistry());
              if (!e)
                  return;
              SerializeEntity(emitter, e);
          });
        emitter << YAML::EndSeq;
        emitter << YAML::EndMap;

        std::ofstream f(filename);
        f << emitter.c_str();
    }

    bool SceneSerializer::DeserializeText(const std::string& filename)
    {
        YAML::Node data = YAML::LoadFile(filename);
        if (!data["Scene"])
            return false;
        YAML::Node entities = data["Entities"];
        if (entities)
        {
            for (YAML::Node entity : entities)
            {
                std::string name;
                YAML::Node tagComponent = entity["TagComponent"];
                if (tagComponent)
                    name = tagComponent["Tag"].as<std::string>();

                uint64_t layer = 1;
                YAML::Node layerMaskComponent = entity["LayerMask"];
                if (layerMaskComponent)
                    layer = layerMaskComponent.as<uint64_t>();

                bool enabled = true;
                YAML::Node entityEnabled = entity["Enabled"];
                if (entityEnabled)
                    enabled = entityEnabled.as<bool>();

                Entity newEntity = m_Scene->CreateEntity(name, layer);
                newEntity.SetEnabled(enabled);

                DeserializeEntity(entity, newEntity);
            }
        }
        return true;
    }

}

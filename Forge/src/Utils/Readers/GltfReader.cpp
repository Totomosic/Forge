#include "ForgePch.h"
#include "GltfReader.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <unordered_set>

namespace Forge
{

    ShaderDataType GetShaderDataType(int gltfType, int gltfComponentType)
    {
        if (gltfType == TINYGLTF_TYPE_SCALAR)
        {
            switch (gltfComponentType)
            {
            case TINYGLTF_COMPONENT_TYPE_FLOAT:
                return ShaderDataType::Float;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                return ShaderDataType::Uint;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                return ShaderDataType::Ushort;
            case TINYGLTF_COMPONENT_TYPE_INT:
                return ShaderDataType::Int;
            default:
                break;
            }
        }
        else if (gltfType == TINYGLTF_TYPE_VEC2)
        {
            switch (gltfComponentType)
            {
            case TINYGLTF_COMPONENT_TYPE_FLOAT:
                return ShaderDataType::Float2;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                return ShaderDataType::Uint2;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                return ShaderDataType::Ushort2;
            case TINYGLTF_COMPONENT_TYPE_INT:
                return ShaderDataType::Int2;
            default:
                break;
            }
        }
        else if (gltfType == TINYGLTF_TYPE_VEC3)
        {
            switch (gltfComponentType)
            {
            case TINYGLTF_COMPONENT_TYPE_FLOAT:
                return ShaderDataType::Float3;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                return ShaderDataType::Uint3;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                return ShaderDataType::Ushort3;
            case TINYGLTF_COMPONENT_TYPE_INT:
                return ShaderDataType::Int3;
            default:
                break;
            }
        }
        else if (gltfType == TINYGLTF_TYPE_VEC4)
        {
            switch (gltfComponentType)
            {
            case TINYGLTF_COMPONENT_TYPE_FLOAT:
                return ShaderDataType::Float4;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                return ShaderDataType::Uint4;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                return ShaderDataType::Ushort4;
            case TINYGLTF_COMPONENT_TYPE_INT:
                return ShaderDataType::Int4;
            default:
                break;
            }
        }
        else if (gltfType == TINYGLTF_TYPE_MAT2)
        {
            switch (gltfComponentType)
            {
            case TINYGLTF_COMPONENT_TYPE_FLOAT:
                return ShaderDataType::Mat2;
            default:
                break;
            }
        }
        else if (gltfType == TINYGLTF_TYPE_MAT3)
        {
            switch (gltfComponentType)
            {
            case TINYGLTF_COMPONENT_TYPE_FLOAT:
                return ShaderDataType::Mat3;
            default:
                break;
            }
        }
        else if (gltfType == TINYGLTF_TYPE_MAT4)
        {
            switch (gltfComponentType)
            {
            case TINYGLTF_COMPONENT_TYPE_FLOAT:
                return ShaderDataType::Mat4;
            default:
                break;
            }
        }
        FORGE_ASSERT(false, "Invalid component type");
        return ShaderDataType::Float;
    }

    glm::mat4 CreateMatrix(const std::vector<double>& matrix, const std::vector<double>& translation, const std::vector<double>& rotation, const std::vector<double>& scale)
    {
        if (matrix.size() == 16)
        {
            return glm::mat4(
                float(matrix[0]), float(matrix[1]), float(matrix[2]), float(matrix[3]),
                float(matrix[4]), float(matrix[5]), float(matrix[6]), float(matrix[7]),
                float(matrix[8]), float(matrix[9]), float(matrix[10]), float(matrix[11]),
                float(matrix[12]), float(matrix[13]), float(matrix[14]), float(matrix[15])
            );
        }
        if (translation.size() == 3 && rotation.size() == 4 && scale.size() == 3)
        {
            return glm::translate(glm::mat4(1.0f), glm::vec3{ float(translation[0]), float(translation[1]), float(translation[2]) }) * glm::toMat4(glm::quat(float(rotation[3]), float(rotation[0]), float(rotation[1]), float(rotation[2])));
        }
        return glm::mat4(1.0f);
    }

    glm::mat4 CreateMatrix(const tinygltf::Model& model, const tinygltf::Accessor& accessor, int index)
    {
        FORGE_ASSERT(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT, "Invalid Component Type");
        FORGE_ASSERT(accessor.type == TINYGLTF_TYPE_MAT4, "Invalid Type");
        const auto& view = model.bufferViews[accessor.bufferView];
        const float* data = (const float*)&model.buffers[view.buffer].data[view.byteOffset + accessor.byteOffset + 16 * sizeof(float) * index];
        return glm::mat4(
            float(data[0]), float(data[1]), float(data[2]), float(data[3]),
            float(data[4]), float(data[5]), float(data[6]), float(data[7]),
            float(data[8]), float(data[9]), float(data[10]), float(data[11]),
            float(data[12]), float(data[13]), float(data[14]), float(data[15])
        );
    }

    Scope<Joint> ProcessJoint(const tinygltf::Model& model, int nodeIndex, const glm::mat4& parentTransform, const std::unordered_set<int>& jointNodes, const std::vector<int>& jointOrder, std::unordered_map<int, Joint*>& jointsById, int& jointCount)
    {
        jointCount++;
        const auto& jointData = model.nodes[nodeIndex];
        Scope<Joint> joint = CreateScope<Joint>();
        joint->Transform = parentTransform * CreateMatrix(jointData.matrix, jointData.translation, jointData.rotation, jointData.scale);
        auto it = std::find(jointOrder.begin(), jointOrder.end(), nodeIndex);
        FORGE_ASSERT(it != jointOrder.end(), "Invalid joint");
        joint->Id = int(it - jointOrder.begin());
        jointsById[joint->Id] = joint.get();
        joint->InverseBindTransform = parentTransform;// *CreateMatrix(jointData.matrix, jointData.translation, jointData.rotation, jointData.scale);

        for (int index : jointData.children)
        {
            if (jointNodes.find(index) != jointNodes.end())
                joint->Children.push_back(ProcessJoint(model, index, joint->Transform, jointNodes, jointOrder, jointsById, jointCount));
        }
        return joint;
    }

    Scope<Joint> ProcessSkin(const tinygltf::Model& model, const tinygltf::Skin& skin, const std::unordered_set<int>& jointNodes, const std::vector<int>& jointOrder, std::unordered_map<int, Joint*>& jointsById, int& jointCount)
    {
        jointCount = 0;
        return ProcessJoint(model, skin.skeleton, glm::mat4(1.0f), jointNodes, jointOrder, jointsById, jointCount);
    }

    void UpdateJointTransforms(Joint* joint, const glm::mat4& parentTransform)
    {
        joint->Transform = parentTransform * joint->Transform;
        for (const auto& child : joint->Children)
            UpdateJointTransforms(child.get(), joint->Transform);
    }

    float GetFloatValue(const tinygltf::Model& model, const tinygltf::Accessor& accessor, int index)
    {
        FORGE_ASSERT(accessor.type == TINYGLTF_TYPE_SCALAR && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT, "Invalid accessor type");
        const auto& view = model.bufferViews[accessor.bufferView];
        const auto& buffer = model.buffers[view.buffer];
        return *(float*)&buffer.data[view.byteOffset + accessor.byteOffset + index * sizeof(float)];
    }

    glm::vec3 GetVec3Value(const tinygltf::Model& model, const tinygltf::Accessor& accessor, int index)
    {
        FORGE_ASSERT(accessor.type == TINYGLTF_TYPE_VEC3 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT, "Invalid accessor type");
        const auto& view = model.bufferViews[accessor.bufferView];
        const auto& buffer = model.buffers[view.buffer];
        return glm::vec3{
            *(float*)&buffer.data[view.byteOffset + accessor.byteOffset + (size_t(index) * 3 + 0) * sizeof(float)],
            *(float*)&buffer.data[view.byteOffset + accessor.byteOffset + (size_t(index) * 3 + 1) * sizeof(float)],
            *(float*)&buffer.data[view.byteOffset + accessor.byteOffset + (size_t(index) * 3 + 2) * sizeof(float)],
        };
    }

    glm::quat GetQuatValue(const tinygltf::Model& model, const tinygltf::Accessor& accessor, int index)
    {
        FORGE_ASSERT(accessor.type == TINYGLTF_TYPE_VEC4 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT, "Invalid accessor type");
        const auto& view = model.bufferViews[accessor.bufferView];
        const auto& buffer = model.buffers[view.buffer];
        return glm::quat(
             *(float*)&buffer.data[view.byteOffset + accessor.byteOffset + (size_t(index) * 4 + 3) * sizeof(float)],
             *(float*)&buffer.data[view.byteOffset + accessor.byteOffset + (size_t(index) * 4 + 0) * sizeof(float)],
             *(float*)&buffer.data[view.byteOffset + accessor.byteOffset + (size_t(index) * 4 + 1) * sizeof(float)],
             *(float*)&buffer.data[view.byteOffset + accessor.byteOffset + (size_t(index) * 4 + 2) * sizeof(float)]
        );
    }

    void AddKeyFrame(const tinygltf::Model& model, std::unordered_map<float, AnimationKeyFrame>& keyFrames, const tinygltf::Accessor& timeAccessor, const tinygltf::Accessor& propertyAccessor, const std::string& path, int jointId, int jointCount)
    {
        if (path == "scale" || path == "weights")
        {
            return;
        }
        FORGE_ASSERT(timeAccessor.count == propertyAccessor.count, "Invalid accessors");
        for (int i = 0; i < timeAccessor.count; i++)
        {
            float time = GetFloatValue(model, timeAccessor, i);
            AnimationKeyFrame& frame = keyFrames[time];
            if (frame.Transforms.empty())
            {
                frame.TimeStamp = time;
                frame.Transforms.resize(jointCount);
                for (int i = 0; i < jointCount; i++)
                    frame.Transforms[i] = { glm::vec3{ 0, 0, 0}, glm::quat{ 0, 0, 0, 1.0f } };
            }
            if (path == "translation")
            {
                frame.Transforms[jointId].Translation = GetVec3Value(model, propertyAccessor, i);
            }
            else
            {
                frame.Transforms[jointId].Orientation = GetQuatValue(model, propertyAccessor, i);
            }
        }
    }



    GltfReader::GltfReader(const std::string& filename)
        : m_Meshes()
    {
        ReadGltf(filename);
    }

    const std::vector<Ref<Mesh>>& GltfReader::GetMeshes() const
    {
        return m_Meshes;
    }

    void GltfReader::ReadGltf(const std::string& filename)
    {
        tinygltf::Model model;
        tinygltf::TinyGLTF loader;

        std::string error;
        std::string warning;

        bool success = loader.LoadASCIIFromFile(&model, &error, &warning, filename);
        if (!success)
        {
            FORGE_ERROR(error);
            return;
        }

        if (!warning.empty())
        {
            FORGE_WARN(warning);
        }

        std::unordered_set<int> jointNodes;
        for (const auto& skin : model.skins)
        {
            for (int index : skin.joints)
            {
                jointNodes.insert(index);
            }
        }

        std::unordered_map<int, int> nodeIndexToJointId;

        // Meshes
        for (const auto& node : model.nodes)
        {
            if (node.mesh >= 0)
            {
                const auto& mesh = model.meshes[node.mesh];
                for (const auto& primitive : mesh.primitives)
                {
                    const auto& indexAccessor = model.accessors[primitive.indices];
                    const auto& indexView = model.bufferViews[indexAccessor.bufferView];
                    const auto& indexBuffer = model.buffers[indexView.buffer];

                    const uint32_t* indices = (const uint32_t*)(&indexBuffer.data[indexView.byteOffset + indexAccessor.byteOffset]);

                    Ref<VertexArray> vao = VertexArray::Create();

                    struct VertexBufferInfo
                    {
                    public:
                        const void* Data;
                        size_t Size;
                        int Index;
                        BufferLayout Layout;
                    };

                    std::vector<VertexBufferInfo> vbos;
                    int index = 0;

                    for (const char* attribute : { "POSITION", "NORMAL", "TEXCOORD_0", "TANGENT", "JOINTS_0", "WEIGHTS_0" })
                    {
                        if (primitive.attributes.find(attribute) != primitive.attributes.end())
                        {
                            const auto& accessor = model.accessors[primitive.attributes.at(attribute)];
                            const auto& view = model.bufferViews[accessor.bufferView];
                            const auto& buffer = model.buffers[view.buffer];

                            ShaderDataType type = GetShaderDataType(accessor.type, accessor.componentType);

                            BufferLayout layout{
                                { type }
                            };

                            vbos.push_back({ (const void*)&buffer.data[view.byteOffset + accessor.byteOffset], GetTypeSize(type) * accessor.count, index, layout });
                        }
                        index++;
                    }

                    for (const auto& info : vbos)
                    {
                        Ref<VertexBuffer> vbo = VertexBuffer::Create(info.Data, info.Size, info.Layout);
                        vao->AddVertexBuffer(info.Index, vbo);
                    }
                    Ref<IndexBuffer> ibo = IndexBuffer::Create(indices, sizeof(uint32_t) * indexAccessor.count, GetShaderDataType(indexAccessor.type, indexAccessor.componentType));
                    vao->SetIndexBuffer(ibo);

                    // Skeleton
                    if (node.skin >= 0)
                    {
                        const auto& skin = model.skins[node.skin];
                        int jointCount = 0;
                        if (jointNodes.find(skin.skeleton) != jointNodes.end())
                        {
                            std::unordered_map<int, Joint*> jointsById;
                            Scope<Joint> joint = ProcessSkin(model, skin, jointNodes, skin.joints, jointsById, jointCount);

                            for (int i = 0; i < skin.joints.size(); i++)
                            {
                                nodeIndexToJointId[skin.joints[i]] = i;
                            }

                            if (skin.inverseBindMatrices >= 0)
                            {
                                const auto& accessor = model.accessors[skin.inverseBindMatrices];
                                FORGE_ASSERT(accessor.count == jointCount, "Invalid");
                                for (int i = 0; i < accessor.count; i++)
                                {
                                    Joint* target = jointsById.at(i);
                                    target->InverseBindTransform = glm::inverse(target->Transform) * CreateMatrix(model, accessor, i);
                                }
                            }

                            // UpdateJointTransforms(joint.get(), glm::mat4(1.0f));

                            m_Meshes.push_back(CreateRef<AnimatedMesh>(vao, CreateRef<Skeleton>(std::move(joint), jointCount)));
                            continue;
                        }
                    }
                    m_Meshes.push_back(CreateRef<Mesh>(vao));
                }
            }
        }

        // Animations
        for (const auto& animation : model.animations)
        {
            Ref<Animation> anim = CreateRef<Animation>();
            std::unordered_map<float, AnimationKeyFrame> keyFrames;
            for (const auto& channel : animation.channels)
            {
                const auto& sampler = animation.samplers[channel.sampler];
                const auto& timeAccessor = model.accessors[sampler.input];
                const auto& propertyAccessor = model.accessors[sampler.output];
                AddKeyFrame(model, keyFrames, timeAccessor, propertyAccessor, channel.target_path, nodeIndexToJointId.at(channel.target_node), nodeIndexToJointId.size());
            }

            for (const auto& frame : keyFrames)
            {
                anim->KeyFrames.push_back(frame.second);
            }
            std::sort(anim->KeyFrames.begin(), anim->KeyFrames.end(), [](const AnimationKeyFrame& a, const AnimationKeyFrame& b)
            {
                return a.TimeStamp < b.TimeStamp;
            });

            m_Animations[animation.name] = anim;
        }
    }

}

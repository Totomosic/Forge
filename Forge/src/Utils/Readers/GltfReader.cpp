#include "ForgePch.h"
#include "GltfReader.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

namespace Forge
{

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

        FORGE_WARN(warning);
        
        for (const auto& mesh : model.meshes)
        {
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

                constexpr int SizeMultipliers[] = { 3 * sizeof(float), 3 * sizeof(float), 2 * sizeof(float) };
                constexpr ShaderDataType DataTypes[] = { ShaderDataType::Float3, ShaderDataType::Float3, ShaderDataType::Float2 };

                for (const char* attribute : { "POSITION", "NORMAL", "TEXCOORD_0" })
                {
                    if (primitive.attributes.find(attribute) != primitive.attributes.end())
                    {
                        const auto& accessor = model.accessors[primitive.attributes.at(attribute)];
                        const auto& view = model.bufferViews[accessor.bufferView];
                        const auto& buffer = model.buffers[view.buffer];

                        BufferLayout layout;
                        layout.AddAttribute({ DataTypes[index] });

                        vbos.push_back({ (const void*)&buffer.data[view.byteOffset + accessor.byteOffset], SizeMultipliers[index] * accessor.count, index, layout });
                    }
                    index++;
                }

                for (const auto& info : vbos)
                {
                    Ref<VertexBuffer> vbo = VertexBuffer::Create(info.Data, info.Size, info.Layout);
                    vao->AddVertexBuffer(info.Index, vbo);
                }
                Ref<IndexBuffer> ibo = IndexBuffer::Create(indices, sizeof(uint32_t) * indexAccessor.count);
                vao->SetIndexBuffer(ibo);

                m_Meshes.push_back(CreateRef<Mesh>(vao));
            }
        }
    }

}

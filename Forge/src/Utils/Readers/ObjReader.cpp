#include "ForgePch.h"
#include "ObjReader.h"

#include "../FileUtils.h"

#include <charconv>

namespace Forge
{

    void from_chars(const char* start, const char* end, float& value)
    {
#ifdef FORGE_PLATFORM_WINDOWS
        std::from_chars(start, end, value);
#else
        FORGE_ASSERT(false, "Not implemented");
#endif
    }

    void from_chars(const char* start, const char* end, int64_t& value)
    {
#ifdef FORGE_PLATFORM_WINDOWS
        std::from_chars(start, end, value);
#else
        FORGE_ASSERT(false, "Not implemented");
#endif
    }

    struct FaceIndices
    {
    public:
        int64_t Vertex;
        int64_t TexCoord;
        int64_t Normal;
    };

    struct Face
    {
    public:
        FaceIndices Indices[3];
    };

    bool BeginsWith(const std::string_view& str, const std::string_view& token)
    {
        return str.substr(0, token.size()) == token;
    }

    void HandleVertex(const std::string_view& line, std::vector<glm::vec3>& vertices)
    {
        size_t firstSpace = line.find_first_of(' ');
        size_t secondSpace = line.find_first_of(' ', firstSpace + 1);
        size_t thirdSpace = line.find_first_of(' ', secondSpace + 1);
        FORGE_ASSERT(firstSpace != std::string_view::npos && secondSpace != std::string_view::npos &&
                       thirdSpace != std::string_view::npos,
          "Invalid vertex line");
        glm::vec3 vertex;
        std::string_view x = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
        std::string_view y = line.substr(secondSpace + 1, thirdSpace - secondSpace - 1);
        std::string_view z = line.substr(thirdSpace + 1);
        from_chars(x.data(), x.data() + x.size(), vertex.x);
        from_chars(y.data(), y.data() + y.size(), vertex.y);
        from_chars(z.data(), z.data() + z.size(), vertex.z);
        vertices.push_back(vertex);
    }

    void HandleVertexNormal(const std::string_view& line, std::vector<glm::vec3>& normals)
    {
        size_t firstSpace = line.find_first_of(' ');
        size_t secondSpace = line.find_first_of(' ', firstSpace + 1);
        size_t thirdSpace = line.find_first_of(' ', secondSpace + 1);
        FORGE_ASSERT(firstSpace != std::string_view::npos && secondSpace != std::string_view::npos &&
                       thirdSpace != std::string_view::npos,
          "Invalid normal line");
        glm::vec3 normal;
        std::string_view x = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
        std::string_view y = line.substr(secondSpace + 1, thirdSpace - secondSpace - 1);
        std::string_view z = line.substr(thirdSpace + 1);
        from_chars(x.data(), x.data() + x.size(), normal.x);
        from_chars(y.data(), y.data() + y.size(), normal.y);
        from_chars(z.data(), z.data() + z.size(), normal.z);
        normals.push_back(normal);
    }

    void HandleTexCoord(const std::string_view& line, std::vector<glm::vec2>& texCoords)
    {
        size_t firstSpace = line.find_first_of(' ');
        size_t secondSpace = line.find_first_of(' ', firstSpace + 1);
        FORGE_ASSERT(
          firstSpace != std::string_view::npos && secondSpace != std::string_view::npos, "Invalid texcoord line");
        glm::vec2 texCoord;
        std::string_view x = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
        std::string_view y = line.substr(secondSpace + 1);
        from_chars(x.data(), x.data() + x.size(), texCoord.x);
        from_chars(y.data(), y.data() + y.size(), texCoord.y);
        texCoords.push_back(texCoord);
    }

    FaceIndices ReadFaceBlock(const std::string_view& block)
    {
        size_t firstSlash = block.find_first_of('/');
        size_t secondSlash = block.find_last_of('/');
        std::string_view vertexIndex = block.substr(0, firstSlash);
        std::string_view texCoordIndex = block.substr(firstSlash + 1, secondSlash - firstSlash - 1);
        std::string_view normalIndex = block.substr(secondSlash + 1);
        FaceIndices result;
        from_chars(vertexIndex.data(), vertexIndex.data() + vertexIndex.size(), result.Vertex);
        if (texCoordIndex.empty())
            result.TexCoord = -1;
        else
            from_chars(texCoordIndex.data(), texCoordIndex.data() + texCoordIndex.size(), result.TexCoord);
        if (normalIndex.empty())
            result.Normal = -1;
        else
            from_chars(normalIndex.data(), normalIndex.data() + normalIndex.size(), result.Normal);
        return result;
    }

    void AddFace(const FaceIndices& b0, const FaceIndices& b1, const FaceIndices& b2, std::vector<Face>& faces)
    {
        Face& face = faces.emplace_back();
        face.Indices[0] = b0;
        face.Indices[1] = b1;
        face.Indices[2] = b2;
    }

    void HandleFace(const std::string_view& line, std::vector<Face>& faces)
    {
        size_t firstSpace = line.find_first_of(' ');
        size_t secondSpace = line.find_first_of(' ', firstSpace + 1);
        size_t thirdSpace = line.find_first_of(' ', secondSpace + 1);
        FORGE_ASSERT(firstSpace != std::string_view::npos && secondSpace != std::string_view::npos &&
                       thirdSpace != std::string_view::npos,
          "Invalid face line");
        size_t fourthSpace = line.find_first_of(' ', thirdSpace + 1);

        FaceIndices b0 = ReadFaceBlock(line.substr(firstSpace + 1, secondSpace - firstSpace - 1));
        FaceIndices b1 = ReadFaceBlock(line.substr(secondSpace + 1, thirdSpace - secondSpace - 1));
        FaceIndices b2 = ReadFaceBlock(line.substr(thirdSpace + 1, fourthSpace - thirdSpace - 1));

        AddFace(b0, b1, b2, faces);
        if (fourthSpace != std::string_view::npos)
        {
            FaceIndices b3 = ReadFaceBlock(line.substr(fourthSpace + 1));
            AddFace(b0, b2, b3, faces);
        }
    }

    ObjReader::ObjReader(const std::string& filename) : m_Mesh()
    {
        ReadObj(filename);
    }

    void ObjReader::ReadObj(const std::string& filename)
    {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;
        std::vector<Face> faces;

        std::string dataString = FileUtils::ReadTextFile(filename);
        std::string_view data = dataString;
        size_t lineStart = data.find_first_not_of("\r\n");
        size_t lineEnd = data.find_first_of("\r\n");

        while (lineEnd != std::string::npos)
        {
            std::string_view line = data.substr(lineStart, lineEnd - lineStart);
            if (line.size() > 0)
            {
                if (BeginsWith(line, "vn"))
                    HandleVertexNormal(line, normals);
                else if (BeginsWith(line, "vt"))
                    HandleTexCoord(line, texCoords);
                else if (line[0] == 'v')
                    HandleVertex(line, vertices);
                else if (line[0] == 'f')
                    HandleFace(line, faces);
            }
            lineStart = data.find_first_not_of("\r\n", lineEnd);
            lineEnd = data.find_first_of("\r\n", lineStart);
        }

        int vertexSize = 3 + 3 + 2;
        float* vertexData = new float[faces.size() * 3 * vertexSize];
        uint32_t* indices = new uint32_t[faces.size() * 3];

        for (size_t i = 0; i < faces.size(); i++)
        {
            const Face& face = faces[i];
            indices[i * 3 + 0] = i * 3 + 0;
            indices[i * 3 + 1] = i * 3 + 1;
            indices[i * 3 + 2] = i * 3 + 2;

            for (int j = 0; j < 3; j++)
            {
                const glm::vec3& position = vertices[face.Indices[j].Vertex - 1];
                glm::vec3 normal = face.Indices[j].Normal < 0 || face.Indices[j].Normal >= normals.size() ?
                                     glm::vec3 {} :
                                     normals[face.Indices[j].Normal - 1];
                glm::vec2 texCoord =
                  face.Indices[j].TexCoord < 0 ? glm::vec2 {} : texCoords[face.Indices[j].TexCoord - 1];

                vertexData[(i * 3 + j) * vertexSize + 0] = position.x;
                vertexData[(i * 3 + j) * vertexSize + 1] = position.y;
                vertexData[(i * 3 + j) * vertexSize + 2] = position.z;
                vertexData[(i * 3 + j) * vertexSize + 3] = normal.x;
                vertexData[(i * 3 + j) * vertexSize + 4] = normal.y;
                vertexData[(i * 3 + j) * vertexSize + 5] = normal.z;
                vertexData[(i * 3 + j) * vertexSize + 6] = texCoord.x;
                vertexData[(i * 3 + j) * vertexSize + 7] = texCoord.y;
            }
        }

        BufferLayout layout = {
          {ShaderDataType::Float3},
          {ShaderDataType::Float3},
          {ShaderDataType::Float2},
        };

        Ref<VertexBuffer> vbo = VertexBuffer::Create(vertexData, faces.size() * 3 * vertexSize * sizeof(float), layout);
        Ref<IndexBuffer> ibo = IndexBuffer::Create(indices, faces.size() * 3 * sizeof(uint32_t));
        Ref<VertexArray> vao = VertexArray::Create();
        vao->AddVertexBuffer(vbo);
        vao->SetIndexBuffer(ibo);
        m_Mesh = CreateRef<Mesh>(vao);

        delete[] indices;
        delete[] vertexData;
    }

}

#include "ForgePch.h"
#include "Renderer2D.h"
#include "Assets/GraphicsCache.h"

namespace Forge
{

	static constexpr glm::vec3 s_QuadPositions[4] = {
		glm::vec3{ -0.5f,  0.5f, 0.0f },
		glm::vec3{ -0.5f, -0.5f, 0.0f },
		glm::vec3{  0.5f, -0.5f, 0.0f },
		glm::vec3{  0.5f,  0.5f, 0.0f },
	};

	static constexpr glm::vec2 s_TexCoords[4] = {
		glm::vec2{ 0.0f, 1.0f },
		glm::vec2{ 0.0f, 0.0f },
		glm::vec2{ 1.0f, 0.0f },
		glm::vec2{ 1.0f, 1.0f },
	};

	Renderer2D::Renderer2D()
		: m_UsedIndices(0), m_CurrentVertexIndex(0), m_CurrentTextureIndex(1), m_Vertices(), m_Indices(), m_Shader(), m_ModelIndex(0), m_Models()
	{
		Init();
	}

	void Renderer2D::BeginScene()
	{
		m_ModelIndex = 0;
		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		EndBatch();
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Color& color)
	{
		DrawQuad(position, size, nullptr, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const Color& color)
	{
		if (m_UsedIndices >= MaxIndices)
		{
			EndBatch();
			StartBatch();
		}
		for (int i = 0; i < 4; i++)
		{
			m_Vertices[m_CurrentVertexIndex].Position = position + s_QuadPositions[i] * glm::vec3{ size.x, size.y, 1.0f };
			m_Vertices[m_CurrentVertexIndex].TexCoord = s_TexCoords[i];
			m_Vertices[m_CurrentVertexIndex].Color = color;
			m_Vertices[m_CurrentVertexIndex].TextureId = BindTexture(texture);
			m_CurrentVertexIndex++;
		}
		m_UsedIndices += 6;
	}

	void Renderer2D::Init()
	{
#include "Assets/Shaders/BatchTexture.h"
		m_Shader = Shader::CreateFromSource(vertexShaderSource, fragmentShaderSource);

		m_Vertices = std::make_unique<QuadVertex[]>(MaxVertices);
		m_Indices = std::make_unique<uint32_t[]>(MaxIndices);

		uint32_t offset = 0;
		for (uint32_t i = 0; i < MaxIndices; i += 6)
		{
			m_Indices[i + 0] = offset + 0;
			m_Indices[i + 1] = offset + 1;
			m_Indices[i + 2] = offset + 2;

			m_Indices[i + 3] = offset + 0;
			m_Indices[i + 4] = offset + 2;
			m_Indices[i + 5] = offset + 3;
			offset += 4;
		}

		m_Models.push_back(CreateModel());
	}

	void Renderer2D::StartBatch()
	{
		m_UsedIndices = 0;
		m_CurrentVertexIndex = 0;
	}

	void Renderer2D::EndBatch()
	{
		if (m_UsedIndices > 0)
		{
			Ref<VertexArray> vertices = m_Models[m_ModelIndex]->GetSubModels()[0].Mesh->GetVertices();
			vertices->GetVertexBuffer(0)->SetData(m_Vertices.get(), m_CurrentVertexIndex * sizeof(QuadVertex));
			vertices->SetMaxIndices(m_UsedIndices);
			m_ModelIndex++;
		}
	}

	Ref<Model> Renderer2D::CreateModel()
	{
		BufferLayout layout = {
			{ ShaderDataType::Float3 },
			{ ShaderDataType::Float2 },
			{ ShaderDataType::Float4 },
			{ ShaderDataType::Int },
		};
		Ref<VertexBuffer> vbo = VertexBuffer::Create(MaxVertices * sizeof(QuadVertex), layout);
		Ref<IndexBuffer> ibo = IndexBuffer::Create(m_Indices.get(), MaxIndices * sizeof(uint32_t));
		Ref<VertexArray> vao = VertexArray::Create();
		vao->AddVertexBuffer(vbo);
		vao->SetIndexBuffer(ibo);
		Ref<Material> material = Material::Create(m_Shader);
		material->GetUniforms().SetUniform("u_Textures[0]", GraphicsCache::WhiteTexture());
		return Model::Create(CreateRef<Mesh>(vao), material);
	}

	int Renderer2D::BindTexture(const Ref<Texture2D>& texture)
	{
		if (!texture)
			return 0;
		auto it = m_TextureIndexMap.find(texture.get());
		if (it != m_TextureIndexMap.end())
			return it->second;
		int index = m_CurrentTextureIndex++;
		m_TextureIndexMap[texture.get()] = index;
		m_Models[m_ModelIndex]->GetSubModels()[0].Material->GetUniforms().SetUniform("u_Textures[" + std::to_string(index) + ']', texture);
		return index;
	}

}

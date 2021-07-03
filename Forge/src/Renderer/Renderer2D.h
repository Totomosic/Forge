#pragma once
#include "Model.h"
#include "RendererContext.h"

namespace Forge
{

	class FORGE_API Renderer2D
	{
	private:
		static constexpr uint32_t MaxQuads = 20000;
		static constexpr uint32_t MaxVertices = MaxQuads * 4;
		static constexpr uint32_t MaxIndices = MaxQuads * 6;

		struct FORGE_API QuadVertex
		{
		public:
			glm::vec3 Position;
			glm::vec2 TexCoord;
			Forge::Color Color;
			int TextureId;
		};

	private:
		uint32_t m_UsedIndices;
		uint32_t m_CurrentVertexIndex;
		uint32_t m_CurrentTextureIndex;

		std::unordered_map<Texture2D*, int> m_TextureIndexMap;
		std::unique_ptr<QuadVertex[]> m_Vertices;
		std::unique_ptr<uint32_t[]> m_Indices;
		Ref<Shader> m_Shader;

		uint32_t m_ModelIndex;
		std::vector<Ref<Model>> m_Models;

	public:
		Renderer2D();

		inline const Ref<Model>* GetRenderables() const { return m_Models.data(); }
		inline uint32_t GetRenderableCount() const { return m_ModelIndex; }

		void BeginScene();
		void EndScene();

		void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Color& color);
		void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const Color& color = COLOR_WHITE);

	private:
		void Init();
		void StartBatch();
		void EndBatch();
		Ref<Model> CreateModel();
		int BindTexture(const Ref<Texture2D>& texture);
	};

}

#pragma once
#include "Buffer.h"

namespace Forge
{

	namespace Detail
	{

		struct FORGE_API VertexArrayDestructor
		{
		public:
			void operator()(uint32_t id) const
			{
				glDeleteVertexArrays(1, &id);
			}
		};

	}

	class FORGE_API VertexArray
	{
	private:
		using Handle = Detail::ScopedId<Detail::VertexArrayDestructor>;

		Handle m_Handle;
		uint32_t m_CurrentAttributeIndex;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;

		uint32_t m_MaxIndices = (uint32_t)-1;

	public:
		VertexArray();

		inline uint32_t GetIndexCount() const { return std::min(m_MaxIndices, m_IndexBuffer->GetCount()); }
		inline const Ref<VertexBuffer>& GetVertexBuffer(int index) const { return m_VertexBuffers[index]; }
		inline const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }
		inline void SetMaxIndices(uint32_t count) { m_MaxIndices = count; }

		void Bind() const;
		void Unbind() const;

		void AddVertexBuffer(const Ref<VertexBuffer>& buffer);
		void AddVertexBuffer(int index, const Ref<VertexBuffer>& buffer);
		void SetIndexBuffer(const Ref<IndexBuffer>& buffer);

	public:
		static Ref<VertexArray> Create();

	private:
		void Init();

	};

}

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

	public:
		VertexArray();

		inline const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

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

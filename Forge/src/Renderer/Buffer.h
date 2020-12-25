#pragma once
#include "Layout.h"

namespace Forge
{

	namespace Detail
	{

		struct FORGE_API BufferDestructor
		{
		public:
			void operator()(uint32_t id) const
			{
				glDeleteBuffers(1, &id);
			}
		};

		template<typename Fn>
		class FORGE_API ScopedId
		{
		public:
			uint32_t Id;

		public:
			ScopedId()
				: Id(0)
			{}

			ScopedId(uint32_t id)
				: Id(id)
			{}

			ScopedId(const ScopedId<Fn>& other) = delete;
			ScopedId<Fn>& operator=(const ScopedId<Fn>& other) = delete;

			ScopedId(ScopedId<Fn>&& other)
				: Id(other.Id)
			{
				other.Id = 0;
			}

			ScopedId<Fn>& operator=(ScopedId<Fn>&& other)
			{
				std::swap(Id, other.Id);
				return *this;
			}

			~ScopedId()
			{
				if (Id != 0)
					Fn()(Id);
			}
		};

	}

	class FORGE_API VertexBuffer
	{
	private:
		using Handle = Detail::ScopedId<Detail::BufferDestructor>;

		Handle m_Handle;
		BufferLayout m_Layout;

	public:
		VertexBuffer();
		VertexBuffer(const void* data, size_t sizeBytes, const BufferLayout& layout);

		inline const BufferLayout& GetLayout() const { return m_Layout; }
		void Bind() const;
		void Unbind() const;

	public:
		static Ref<VertexBuffer> Create(size_t sizeBytes, const BufferLayout& layout);
		static Ref<VertexBuffer> Create(const void* data, size_t sizeBytes, const BufferLayout& layout);

	private:
		void Init(const void* data, size_t sizeBytes);
	};

	class FORGE_API IndexBuffer
	{
	public:
		using Type = uint32_t;

	private:
		using Handle = Detail::ScopedId<Detail::BufferDestructor>;

		Handle m_Handle;
		uint32_t m_IndexCount;

	public:
		IndexBuffer();
		IndexBuffer(const Type* data, size_t sizeBytes);

		inline uint32_t GetCount() const { return m_IndexCount; }

		void Bind() const;
		void Unbind() const;

	public:
		static Ref<IndexBuffer> Create(size_t sizeBytes);
		static Ref<IndexBuffer> Create(const Type* data, size_t sizeBytes);

	private:
		void Init(const Type* data, size_t sizeBytes);
	};

}

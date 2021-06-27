#pragma once
#include "Buffer.h"

namespace Forge
{

	class FORGE_API UniformBuffer
	{
	private:
		using Handle = Detail::ScopedId<Detail::BufferDestructor>;

		Handle m_Handle;

	public:
		UniformBuffer(uint32_t size, uint32_t binding);

		void SetData(const void* data, uint32_t size, uint32_t offset = 0);

	public:
		static Ref<UniformBuffer> Create(uint32_t size, uint32_t binding);
	};

}

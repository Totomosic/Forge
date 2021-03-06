#pragma once
#include "ForgePch.h"
#include <glad/glad.h>

namespace Forge
{

	enum class ShaderDataType
	{
		Bool,
		Int,
		Int2,
		Int3,
		Int4,
		Uint,
		Uint2,
		Uint3,
		Uint4,
		Ushort,
		Ushort2,
		Ushort3,
		Ushort4,
		Float,
		Float2,
		Float3,
		Float4,
		Mat2,
		Mat3,
		Mat4,

		Sampler1D,
		Sampler2D,
		Sampler3D,
		SamplerCube,
	};

	constexpr uint32_t GetComponentCount(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Bool:
		case ShaderDataType::Int:
		case ShaderDataType::Uint:
		case ShaderDataType::Ushort:
		case ShaderDataType::Float:
			return 1;
		case ShaderDataType::Int2:
		case ShaderDataType::Uint2:
		case ShaderDataType::Ushort2:
		case ShaderDataType::Float2:
			return 2;
		case ShaderDataType::Int3:
		case ShaderDataType::Uint3:
		case ShaderDataType::Ushort3:
		case ShaderDataType::Float3:
			return 3;
		case ShaderDataType::Int4:
		case ShaderDataType::Uint4:
		case ShaderDataType::Ushort4:
		case ShaderDataType::Float4:
			return 4;
		default:
			break;
		}
		FORGE_ASSERT(false, "Invalid data type");
		return 0;
	}

	constexpr uint32_t GetTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Bool:
			return 1 * sizeof(GLboolean);
		case ShaderDataType::Int:
		case ShaderDataType::Uint:
		case ShaderDataType::Float:
			return 1 * sizeof(GLfloat);
		case ShaderDataType::Int2:
		case ShaderDataType::Uint2:
		case ShaderDataType::Float2:
			return 2 * sizeof(GLfloat);
		case ShaderDataType::Int3:
		case ShaderDataType::Uint3:
		case ShaderDataType::Float3:
			return 3 * sizeof(GLfloat);
		case ShaderDataType::Int4:
		case ShaderDataType::Uint4:
		case ShaderDataType::Float4:
			return 4 * sizeof(GLfloat);
		case ShaderDataType::Ushort:
		case ShaderDataType::Ushort2:
		case ShaderDataType::Ushort3:
		case ShaderDataType::Ushort4:
			return GetComponentCount(type) * sizeof(GLushort);
		case ShaderDataType::Mat2:
			return 4 * sizeof(GLfloat);
		case ShaderDataType::Mat3:
			return 9 * sizeof(GLfloat);
		case ShaderDataType::Mat4:
			return 16 * sizeof(GLfloat);
		case ShaderDataType::Sampler1D:
		case ShaderDataType::Sampler2D:
		case ShaderDataType::Sampler3D:
		case ShaderDataType::SamplerCube:
			return 1 * sizeof(int);
		default:
			break;
		}
		FORGE_ASSERT(false, "Invalid data type");
		return 0;
	}

	constexpr uint32_t GetGlType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Bool:
			return GL_BOOL;
		case ShaderDataType::Int:
		case ShaderDataType::Int2:
		case ShaderDataType::Int3:
		case ShaderDataType::Int4:
			return GL_INT;
		case ShaderDataType::Uint:
		case ShaderDataType::Uint2:
		case ShaderDataType::Uint3:
		case ShaderDataType::Uint4:
			return GL_UNSIGNED_INT;
		case ShaderDataType::Ushort:
		case ShaderDataType::Ushort2:
		case ShaderDataType::Ushort3:
		case ShaderDataType::Ushort4:
			return GL_UNSIGNED_SHORT;
		case ShaderDataType::Float:		
		case ShaderDataType::Float2:
		case ShaderDataType::Mat2:		
		case ShaderDataType::Float3:
		case ShaderDataType::Mat3:		
		case ShaderDataType::Float4:
		case ShaderDataType::Mat4:
			return GL_FLOAT;
		default:
			break;
		}
		return GL_FLOAT;
	}

	struct FORGE_API VertexAttribute
	{
	public:
		ShaderDataType Type;
		bool Normalized = false;

		uint32_t GlType;
		size_t Offset;
	};

	class FORGE_API BufferLayout
	{
	private:
		std::vector<VertexAttribute> m_Attributes;
		uint32_t m_Stride = 0;

	public:
		BufferLayout() = default;
		BufferLayout(std::initializer_list<VertexAttribute> attributes);

		void AddAttribute(const VertexAttribute& attribute);

		inline uint32_t GetStride() const { return m_Stride; }

		inline std::vector<VertexAttribute>::iterator begin() { return m_Attributes.begin(); }
		inline std::vector<VertexAttribute>::iterator end() { return m_Attributes.end(); }
		inline std::vector<VertexAttribute>::const_iterator begin() const { return m_Attributes.begin(); }
		inline std::vector<VertexAttribute>::const_iterator end() const { return m_Attributes.end(); }

	public:
		static BufferLayout Default();

	private:
		void Init();
	};

}

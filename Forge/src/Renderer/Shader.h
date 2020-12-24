#pragma once
#include "Buffer.h"

namespace Forge
{

	namespace Detail
	{

		struct FORGE_API ShaderDestructor
		{
		public:
			void operator()(uint32_t id)
			{
				glDeleteProgram(id);
			}
		};

	}

	class FORGE_API Shader
	{
	private:
		using Handle = Detail::ScopedId<Detail::ShaderDestructor>;

		Handle m_Handle;

	public:
		Shader(const std::string& vertexSource, const std::string& fragmentSource);

		void Bind() const;
		void Unbind() const;

	public:
		static Ref<Shader> CreateFromSource(const std::string& vertexSource, const std::string& fragmentSource);
		static Ref<Shader> CreateFromFile(const std::string& vertexFilePath, const std::string& fragmentFilePath);
		static Ref<Shader> CreateFromFile(const std::string& shaderFilePath);

	private:
		void Init(const std::string& vertexSource, const std::string& fragmentSource);

	};

}

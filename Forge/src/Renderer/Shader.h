#pragma once
#include "Buffer.h"
#include "Core/Color.h"

#include <glm/glm.hpp>

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
		std::unordered_map<std::string, int> m_UniformLocations;

	public:
		Shader(const std::string& vertexSource, const std::string& fragmentSource);

		void Bind() const;
		void Unbind() const;

		void SetUniform(const std::string& name, bool value);
		void SetUniform(const std::string& name, int value);
		void SetUniform(const std::string& name, float value);
		void SetUniform(const std::string& name, const glm::vec2& value);
		void SetUniform(const std::string& name, const glm::vec3& value);
		void SetUniform(const std::string& name, const glm::vec4& value);
		void SetUniform(const std::string& name, const Color& value);
		void SetUniform(const std::string& name, const glm::mat2& value);
		void SetUniform(const std::string& name, const glm::mat3& value);
		void SetUniform(const std::string& name, const glm::mat4& value);

		bool UniformExists(const std::string& name) const;

	public:
		static Ref<Shader> CreateFromSource(const std::string& vertexSource, const std::string& fragmentSource);
		static Ref<Shader> CreateFromFile(const std::string& vertexFilePath, const std::string& fragmentFilePath);
		static Ref<Shader> CreateFromFile(const std::string& shaderFilePath);

	private:
		void Init(const std::string& vertexSource, const std::string& fragmentSource);
		int GetUniformLocation(const std::string& name);

	};

}

#pragma once
#include "Buffer.h"
#include "Core/Color.h"

#include <glm/glm.hpp>

namespace Forge
{

	using ShaderDefines = std::vector<std::string>;

	namespace Detail
	{

		struct FORGE_API ShaderDestructor
		{
		public:
			inline void operator()(uint32_t id)
			{
				glUseProgram(0);
				glDeleteProgram(id);
			}
		};
		
		class FORGE_API ShaderSource
		{
		private:
			std::vector<char*> m_Strings;

		public:
			ShaderSource(const std::string& source, const ShaderDefines& defines);
			~ShaderSource();

			inline bool IsValid() const { return Count() > 0; }
			inline size_t Count() const { return m_Strings.size(); }
			inline const char* const* Data() const { return m_Strings.data(); }
		};

	}

	struct FORGE_API UniformDescriptor
	{
	public:
		std::string Name;
		std::string VariableName;
		ShaderDataType Type;
		int Count;
		bool Automatic;
	};

	class FORGE_API Shader
	{
	private:
		using Handle = Detail::ScopedId<Detail::ShaderDestructor>;

		Handle m_Handle;
		std::unordered_map<std::string, int> m_UniformLocations;
		std::vector<UniformDescriptor> m_UniformDescriptors;

	public:
		Shader(const std::string& vertexSource, const std::string& geometrySource, const std::string& fragmentSource, const ShaderDefines& defines = {});

		inline const std::vector<UniformDescriptor>& GetUniformDescriptors() const { return m_UniformDescriptors; }

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
		static Ref<Shader> CreateFromSource(const std::string& vertexSource, const std::string& fragmentSource, const ShaderDefines& defines = {});
		static Ref<Shader> CreateFromFile(const std::string& vertexFilePath, const std::string& fragmentFilePath, const ShaderDefines& defines = {});
		static Ref<Shader> CreateFromSource(const std::string& vertexSource, const std::string& geometrySource, const std::string& fragmentSource, const ShaderDefines& defines = {});
		static Ref<Shader> CreateFromFile(const std::string& vertexFilePath, const std::string& geometryFilePath, const std::string& fragmentFilePath, const ShaderDefines& defines = {});
		static Ref<Shader> CreateFromFile(const std::string& shaderFilePath, const ShaderDefines& defines = {});

	private:
		void Init(const std::string& vertexSource, const std::string& geometrySource, const std::string& fragmentSource, const ShaderDefines& defines);
		void ReflectShader(const std::unordered_map<std::string, std::string>& nameMap);
		int GetUniformLocation(const std::string& name);
		
		std::string PreprocessShaderSource(const std::string& source, const ShaderDefines& defines, std::unordered_map<std::string, std::string>& nameMap);

	};

}

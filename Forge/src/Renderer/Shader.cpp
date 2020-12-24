#include "ForgePch.h"
#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <sstream>

#include "Utils/FileUtils.h"
#include "Utils/StringUtils.h"

namespace Forge
{

    Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource)
        : m_Handle()
    {
        Init(vertexSource, fragmentSource);
    }

    void Shader::Bind() const
    {
        glUseProgram(m_Handle.Id);
    }

    void Shader::Unbind() const
    {
        glUseProgram(0);
    }

    void Shader::SetUniform(const std::string& name, bool value)
    {
        glUniform1i(GetUniformLocation(name), value);
    }

    void Shader::SetUniform(const std::string& name, int value)
    {
        glUniform1i(GetUniformLocation(name), value);
    }

    void Shader::SetUniform(const std::string& name, float value)
    {
        glUniform1f(GetUniformLocation(name), value);
    }

    void Shader::SetUniform(const std::string& name, const glm::vec2& value)
    {
        glUniform2f(GetUniformLocation(name), value.x, value.y);
    }

    void Shader::SetUniform(const std::string& name, const glm::vec3& value)
    {
        glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
    }

    void Shader::SetUniform(const std::string& name, const glm::vec4& value)
    {
        glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
    }

    void Shader::SetUniform(const std::string& name, const Color& value)
    {
        glUniform4f(GetUniformLocation(name), float(value.r) / 255.0f, float(value.g) / 255.0f, float(value.b) / 255.0f, float(value.a) / 255.0f);
    }

    void Shader::SetUniform(const std::string& name, const glm::mat2& value)
    {
        glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetUniform(const std::string& name, const glm::mat3& value)
    {
        glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetUniform(const std::string& name, const glm::mat4& value)
    {
        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    bool Shader::UniformExists(const std::string& name) const
    {
        return glGetUniformLocation(m_Handle.Id, name.c_str()) >= 0;
    }

    Ref<Shader> Shader::CreateFromSource(const std::string& vertexSource, const std::string& fragmentSource)
    {
        return CreateRef<Shader>(vertexSource, fragmentSource);
    }

    Ref<Shader> Shader::CreateFromFile(const std::string& vertexFilePath, const std::string& fragmentFilePath)
    {
        return CreateFromSource(FileUtils::ReadTextFile(vertexFilePath), FileUtils::ReadTextFile(fragmentFilePath));
    }

    Ref<Shader> Shader::CreateFromFile(const std::string& shaderFilePath)
    {
        enum class ShaderType
        {
            NONE,
            VERTEX,
            FRAGMENT,
        };

        std::string fileData = FileUtils::ReadTextFile(shaderFilePath);
        std::stringstream vertexSource;
        std::stringstream fragmentSource;

        ShaderType currentShader = ShaderType::NONE;

        std::vector<std::string> parts = SplitString(fileData, "#shader ");
        for (const std::string& part : parts)
        {
            size_t start = 0;
            size_t end = part.find_first_of("\n \r");
            if (end != std::string::npos)
            {
                std::string type = part.substr(0, end);
                if (type == "vertex" || type == "VERTEX")
                    currentShader = ShaderType::VERTEX;
                else if (type == "fragment" || type == "FRAGMENT")
                    currentShader = ShaderType::FRAGMENT;
                start = end + 1;
                if (start < part.size() && (part[start] == '\r' || part[start] == '\n' || part[start] == ' '))
                    start++;
            }
            if (currentShader == ShaderType::VERTEX)
                vertexSource << part.substr(start);
            else if (currentShader == ShaderType::FRAGMENT)
                fragmentSource << part.substr(start);
        }

        FORGE_INFO("VERTEX SOURCE");
        FORGE_INFO(vertexSource.str());
        FORGE_INFO("FRAGMENT SOURCE");
        FORGE_INFO(fragmentSource.str());

        return CreateFromSource(vertexSource.str(), fragmentSource.str());
    }

    void Shader::Init(const std::string& vertexSource, const std::string& fragmentSource)
    {
        uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
        uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        const char* vSourcePtr = vertexSource.c_str();
        const char* fSourcePtr = fragmentSource.c_str();

        glShaderSource(vertexShader, 1, &vSourcePtr, nullptr);
        glShaderSource(fragmentShader, 1, &fSourcePtr, nullptr);

        glCompileShader(vertexShader);
        glCompileShader(fragmentShader);

        int success;
        char log[512];
        for (uint32_t shader : { vertexShader, fragmentShader })
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
                FORGE_ERROR("Failed compiling shader: {}", shader == vertexShader ? "VERTEX" : "FRAGMENT");
                FORGE_ERROR("{}", log);
            }
        }

        m_Handle.Id = glCreateProgram();
        glAttachShader(m_Handle.Id, vertexShader);
        glAttachShader(m_Handle.Id, fragmentShader);
        glLinkProgram(m_Handle.Id);

        glGetProgramiv(m_Handle.Id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(m_Handle.Id, sizeof(log), nullptr, log);
            FORGE_ERROR("Failed linking shader");
            FORGE_ERROR("{}", log);
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    int Shader::GetUniformLocation(const std::string& name)
    {
        auto it = m_UniformLocations.find(name);
        if (it != m_UniformLocations.end())
            return it->second;
        int location = glGetUniformLocation(m_Handle.Id, name.c_str());
        FORGE_ASSERT(location >= 0, "Unable to find uniform: {}", name);
        m_UniformLocations[name] = location;
        return location;
    }

}

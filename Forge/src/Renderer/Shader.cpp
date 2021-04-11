#include "ForgePch.h"
#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <sstream>

#include "Utils/FileUtils.h"
#include "Utils/StringUtils.h"
#include "ShaderLibrary.h"

namespace Forge
{

    void PreprocessIfDefs(std::string& result, const std::string& identifier, const std::function<bool(const std::string&)>& fn)
    {
        size_t directiveStart = result.find(identifier);
        size_t ifdefLength = identifier.size();
        size_t endifLength = strlen("#endif");
        size_t elseLength = strlen("#else");
        while (directiveStart != std::string::npos)
        {
            size_t directiveEnd = result.find_first_of("\r\n", directiveStart);
            std::string define = result.substr(directiveStart + ifdefLength, directiveEnd - directiveStart - ifdefLength);
            size_t blockEnd = result.find("#endif", directiveStart);
            FORGE_ASSERT(blockEnd != std::string::npos, "Missing #endif");
            size_t elseDirective = result.find("#else", directiveStart);
            bool hasElse = elseDirective != std::string::npos && elseDirective < blockEnd;
            if (fn(define))
            {
                size_t erasedCount = directiveEnd - directiveStart;
                result.erase(directiveStart, erasedCount);
                if (hasElse)
                    result.erase(elseDirective - erasedCount, blockEnd + endifLength - elseDirective);
                else
                    result.erase(blockEnd - erasedCount, endifLength);
            }
            else
            {
                if (hasElse)
                {
                    size_t erasedCount = elseDirective + elseLength - directiveStart;
                    result.erase(directiveStart, erasedCount);
                    result.erase(blockEnd - erasedCount, endifLength);
                }
                else
                    result.erase(directiveStart, blockEnd + endifLength - directiveStart);
            }

            directiveStart = result.find(identifier, directiveStart + 1);
        }
    }

    Shader::Shader(const std::string& vertexSource, const std::string& geometrySource, const std::string& fragmentSource, const ShaderDefines& defines)
        : m_Handle()
    {
        Init(PreprocessShaderSource(vertexSource, defines), PreprocessShaderSource(geometrySource, defines), PreprocessShaderSource(fragmentSource, defines));
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

    Ref<Shader> Shader::CreateFromSource(const std::string& vertexSource, const std::string& fragmentSource, const ShaderDefines& defines)
    {
        return CreateFromSource(vertexSource, "", fragmentSource, defines);
    }

    Ref<Shader> Shader::CreateFromFile(const std::string& vertexFilePath, const std::string& fragmentFilePath, const ShaderDefines& defines)
    {
        return CreateFromSource(FileUtils::ReadTextFile(vertexFilePath), FileUtils::ReadTextFile(fragmentFilePath), defines);
    }

    Ref<Shader> Shader::CreateFromSource(const std::string& vertexSource, const std::string& geometrySource, const std::string& fragmentSource, const ShaderDefines& defines)
    {
        return CreateRef<Shader>(vertexSource, geometrySource, fragmentSource, defines);
    }

    Ref<Shader> Shader::CreateFromFile(const std::string& vertexFilePath, const std::string& geometryFilePath, const std::string& fragmentFilePath, const ShaderDefines& defines)
    {
        return CreateFromSource(FileUtils::ReadTextFile(vertexFilePath), FileUtils::ReadTextFile(geometryFilePath), FileUtils::ReadTextFile(fragmentFilePath), defines);
    }

    Ref<Shader> Shader::CreateFromFile(const std::string& shaderFilePath, const ShaderDefines& defines)
    {
        enum class ShaderType
        {
            NONE,
            VERTEX,
            FRAGMENT,
            GEOMETRY,
        };

        std::string fileData = FileUtils::ReadTextFile(shaderFilePath);
        std::stringstream vertexSource;
        std::stringstream fragmentSource;
        std::stringstream geometrySource;

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
                else if (type == "geometry" || type == "GEOMETRY")
                    currentShader = ShaderType::GEOMETRY;
                start = end + 1;
                if (start < part.size() && (part[start] == '\r' || part[start] == '\n' || part[start] == ' '))
                    start++;
            }
            if (currentShader == ShaderType::VERTEX)
                vertexSource << part.substr(start);
            else if (currentShader == ShaderType::FRAGMENT)
                fragmentSource << part.substr(start);
            else if (currentShader == ShaderType::GEOMETRY)
                geometrySource << part.substr(start);
        }

        return CreateFromSource(vertexSource.str(), geometrySource.str(), fragmentSource.str(), defines);
    }

    void Shader::Init(const std::string& vertexSource, const std::string& geometrySource, const std::string& fragmentSource)
    {
        bool useGeometryShader = !geometrySource.empty();
        FORGE_INFO("VERTEX SHADER SOURCE\n{}", vertexSource);
        if (useGeometryShader)
        {
            FORGE_INFO("GEOMETRY SHADER SOURCE\n{}", geometrySource);
        }
        FORGE_INFO("FRAGMENT SHADER SOURCE\n{}", fragmentSource);

        uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
        uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        uint32_t geometryShader = 0;
        if (useGeometryShader)
            geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

        const char* vSourcePtr = vertexSource.c_str();
        const char* fSourcePtr = fragmentSource.c_str();
        const char* gSourcePtr = geometrySource.c_str();

        glShaderSource(vertexShader, 1, &vSourcePtr, nullptr);
        glShaderSource(fragmentShader, 1, &fSourcePtr, nullptr);
        if (useGeometryShader)
            glShaderSource(geometryShader, 1, &gSourcePtr, nullptr);

        glCompileShader(vertexShader);
        glCompileShader(fragmentShader);
        if (useGeometryShader)
            glCompileShader(geometryShader);

        int success;
        char log[512];
        for (uint32_t shader : { vertexShader, fragmentShader, geometryShader })
        {
            if (shader == geometryShader && !useGeometryShader)
                continue;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
                FORGE_ERROR("Failed compiling shader: {}", shader == vertexShader ? "VERTEX" : shader == fragmentShader ? "FRAGMENT" : "GEOMETRY");
                FORGE_ERROR("{}", log);
            }
        }

        m_Handle.Id = glCreateProgram();
        glAttachShader(m_Handle.Id, vertexShader);
        glAttachShader(m_Handle.Id, fragmentShader);
        if (useGeometryShader)
            glAttachShader(m_Handle.Id, geometryShader);
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
        if (useGeometryShader)
            glDeleteShader(geometryShader);
    }

    int Shader::GetUniformLocation(const std::string& name)
    {
        auto it = m_UniformLocations.find(name);
        if (it != m_UniformLocations.end())
            return it->second;
        int location = glGetUniformLocation(m_Handle.Id, name.c_str());
        if (location < 0)
        {
            FORGE_WARN("Unable to find uniform: {}", name);
        }
        m_UniformLocations[name] = location;
        return location;
    }

    std::string Shader::PreprocessShaderSource(const std::string& source, const ShaderDefines& defines)
    {
        std::string result = source;
        size_t directiveStart = result.find("#include ");
        size_t includeLength = strlen("#include ");
        while (directiveStart != std::string::npos)
        {
            size_t quotePosition = directiveStart + includeLength;
            char quoteCharacter = result[quotePosition];
            char endQuoteCharacter = quoteCharacter;
            if (endQuoteCharacter == '<')
                endQuoteCharacter = '>';
            size_t end = result.find_first_of(endQuoteCharacter, quotePosition + 1);
            FORGE_ASSERT(end != std::string::npos, "Invalid #include directive");

            std::string filename = result.substr(quotePosition + 1, end - quotePosition - 1);
            FORGE_ASSERT(ShaderLibrary::HasShaderSource(filename), "Invalid include file {}", filename);
            result.erase(directiveStart, end - directiveStart + 1);
            result.insert(directiveStart, ShaderLibrary::GetShaderSource(filename));

            directiveStart = result.find("#include ");
        }
        PreprocessIfDefs(result, "#ifdef ", [&defines](const std::string& define) { return std::find(defines.begin(), defines.end(), define) != defines.end(); });
        PreprocessIfDefs(result, "#ifndef ", [&defines](const std::string& define) { return std::find(defines.begin(), defines.end(), define) == defines.end(); });
        PreprocessIfDefs(result, "#if ", [](const std::string& value) { return value == "1" || value == "true"; });
        return result;
    }

}

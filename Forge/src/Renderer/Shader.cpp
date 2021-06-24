#include "ForgePch.h"
#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <sstream>

#include "Utils/FileUtils.h"
#include "Utils/StringUtils.h"
#include "ShaderLibrary.h"

namespace Forge
{

    namespace Detail
    {

        void AddString(std::vector<char*>& vector, const char* string, size_t length = std::string::npos)
        {
            if (length == std::string::npos)
                length = std::strlen(string);
            length++;
            char* buffer = new char[length];
            std::memcpy(buffer, string, length);
            vector.push_back(buffer);
        }

        ShaderSource::ShaderSource(const std::string& source, const ShaderDefines& defines)
            : m_Strings{}
        {
            if (!source.empty())
            {
                AddString(m_Strings, "#version 450 core\n");
                for (const std::string& define : defines)
                {
                    size_t index = define.find_first_of('=');
                    if (index != std::string::npos)
                    {
                        AddString(m_Strings, ("#define " + define.substr(0, index) + " " + define.substr(index + 1) + "\n").c_str());
                    }
                    else
                    {
                        AddString(m_Strings, ("#define " + define + "\n").c_str());
                    }
                }
                AddString(m_Strings, source.c_str(), source.size());
            }
        }

        ShaderSource::~ShaderSource()
        {
            for (char* str : m_Strings)
                delete[] str;
        }

    }

    namespace Utils
    {
        
        ShaderDataType GetTypeFromUniformType(GLenum type)
        {
            switch (type)
            {
            case GL_FLOAT:
                return ShaderDataType::Float;
            case GL_FLOAT_VEC2:
                return ShaderDataType::Float2;
            case GL_FLOAT_VEC3:
                return ShaderDataType::Float3;
            case GL_FLOAT_VEC4:
                return ShaderDataType::Float4;
            case GL_INT:
                return ShaderDataType::Int;
            case GL_INT_VEC2:
                return ShaderDataType::Int2;
            case GL_INT_VEC3:
                return ShaderDataType::Int3;
            case GL_INT_VEC4:
                return ShaderDataType::Int4;
            case GL_BOOL:
                return ShaderDataType::Bool;
            case GL_FLOAT_MAT2:
                return ShaderDataType::Mat2;
            case GL_FLOAT_MAT3:
                return ShaderDataType::Mat3;
            case GL_FLOAT_MAT4:
                return ShaderDataType::Mat4;
            case GL_SAMPLER_1D:
                return ShaderDataType::Sampler1D;
            case GL_SAMPLER_2D:
                return ShaderDataType::Sampler2D;
            case GL_SAMPLER_3D:
                return ShaderDataType::Sampler3D;
            case GL_SAMPLER_CUBE:
                return ShaderDataType::SamplerCube;
            }
            FORGE_ASSERT(false, "Invalid uniform type");
            return ShaderDataType::Float;
        }

    }

    ShaderDataType GetTypeFromGlslString(const std::string& str)
    {
        if (str == "int")
            return ShaderDataType::Int;
        if (str == "float")
            return ShaderDataType::Float;
        if (str == "vec2")
            return ShaderDataType::Float2;
        if (str == "vec3")
            return ShaderDataType::Float3;
        if (str == "vec4")
            return ShaderDataType::Float4;
        FORGE_ASSERT(false, "Unknown GLSL type");
        return ShaderDataType::Float;
    }

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
        : m_Handle(), m_UniformLocations(), m_UniformDescriptors()
    {
        std::unordered_map<std::string, std::string> nameMap;
        Init(PreprocessShaderSource(vertexSource, defines, nameMap), PreprocessShaderSource(geometrySource, defines, nameMap), PreprocessShaderSource(fragmentSource, defines, nameMap), defines);
        ReflectShader(nameMap);
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
        glUniform4f(GetUniformLocation(name), float(value.r), float(value.g), float(value.b), float(value.a));
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

    void Shader::Init(const std::string& vertexSource, const std::string& geometrySource, const std::string& fragmentSource, const ShaderDefines& defines)
    {
        Detail::ShaderSource vertex(vertexSource, defines);
        Detail::ShaderSource fragment(fragmentSource, defines);
        Detail::ShaderSource geometry(geometrySource, defines);

        FORGE_INFO("VERTEX SHADER SOURCE\n{}", vertexSource);
        if (geometry.IsValid())
        {
            FORGE_INFO("GEOMETRY SHADER SOURCE\n{}", geometrySource);
        }
        FORGE_INFO("FRAGMENT SHADER SOURCE\n{}", fragmentSource);

        uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
        uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        uint32_t geometryShader = 0;
        if (geometry.IsValid())
            geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

        glShaderSource(vertexShader, vertex.Count(), vertex.Data(), nullptr);
        glShaderSource(fragmentShader, fragment.Count(), fragment.Data(), nullptr);
        if (geometry.IsValid())
            glShaderSource(geometryShader, geometry.Count(), geometry.Data(), nullptr);

        glCompileShader(vertexShader);
        glCompileShader(fragmentShader);
        if (geometry.IsValid())
            glCompileShader(geometryShader);

        int success;
        char log[512];
        for (uint32_t shader : { vertexShader, fragmentShader, geometryShader })
        {
            if (shader == geometryShader && !geometry.IsValid())
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
        if (geometry.IsValid())
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
        if (geometry.IsValid())
            glDeleteShader(geometryShader);
    }

    void Shader::ReflectShader(const std::unordered_map<std::string, std::string>& nameMap)
    {
        int count;
        glGetProgramiv(m_Handle.Id, GL_ACTIVE_UNIFORMS, &count);
        char nameBuffer[128];
        for (int i = 0; i < count; i++)
        {
            GLenum type;
            UniformDescriptor descriptor;
            glGetActiveUniform(m_Handle.Id, GLuint(i), sizeof(nameBuffer), nullptr, &descriptor.Count, &type, nameBuffer);
            descriptor.VariableName = nameBuffer;
            descriptor.Type = Utils::GetTypeFromUniformType(type);
            descriptor.Automatic = descriptor.VariableName.substr(0, 4) == "frg_";
            if (nameMap.find(descriptor.VariableName) != nameMap.end())
            {
                descriptor.Name = nameMap.at(descriptor.VariableName);
            }
            else
            {
                descriptor.Name = descriptor.VariableName;
            }
            m_UniformDescriptors.push_back(descriptor);
        }
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

    std::string Shader::PreprocessShaderSource(const std::string& source, const ShaderDefines& defines, std::unordered_map<std::string, std::string>& nameMap)
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
        /*PreprocessIfDefs(result, "#ifdef ", [&defines](const std::string& define) { return std::find(defines.begin(), defines.end(), define) != defines.end(); });
        PreprocessIfDefs(result, "#ifndef ", [&defines](const std::string& define) { return std::find(defines.begin(), defines.end(), define) == defines.end(); });
        for (const std::string& def : defines)
        {
            size_t equal = def.find('=');
            if (equal != std::string::npos)
            {
                std::string_view key = def.substr(0, equal);
                std::string_view value = def.substr(equal + 1);
                size_t position = result.find(key);
                while (position != std::string::npos)
                {
                    result.erase(position, key.size());
                    result.insert(position, value);
                    position = result.find(key, position + value.size());
                }
            }
        }
        PreprocessIfDefs(result, "#if ", [](const std::string& value) { return value == "1" || value == "true"; });*/

        // Parse uniform descriptors
        size_t index = result.find("[\"");
        while (index != std::string::npos)
        {
            size_t end = result.find("\"]", index);
            FORGE_ASSERT(end != std::string::npos, "Invalid uniform descriptor");
            std::string name = result.substr(index + 2, end - index - 2);
            size_t uniform = result.find("uniform ", end);
            FORGE_ASSERT(uniform != std::string::npos, "No uniform found for uniform descriptor");
            size_t space = result.find_first_of(' ', uniform + 8);
            FORGE_ASSERT(space != std::string::npos, "Invalid uniform syntax");
            size_t semicolon = result.find_first_of(';', space + 1);
            FORGE_ASSERT(semicolon != std::string::npos, "Invalid uniform syntax");
            std::string varname = result.substr(space + 1, semicolon - space - 1);

            nameMap[varname] = name;

            result.erase(index, end - index + 2);
            index = result.find("[\"", index);
        }

        return result;
    }

}

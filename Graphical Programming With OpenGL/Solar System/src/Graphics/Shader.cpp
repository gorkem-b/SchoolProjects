#include "Graphics/Shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

Shader::Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath)
{
    const std::string vertexSource = ReadTextFile(vertexPath);
    const std::string fragmentSource = ReadTextFile(fragmentPath);

    const GLuint vertexShader = CompileStage(GL_VERTEX_SHADER, vertexSource, vertexPath.u8string());
    const GLuint fragmentShader = CompileStage(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath.u8string());

    m_program = glCreateProgram();
    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);
    glLinkProgram(m_program);

    GLint success = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint infoLogLength = 0;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<GLchar> infoLog(static_cast<std::size_t>(infoLogLength));
        glGetProgramInfoLog(m_program, infoLogLength, nullptr, infoLog.data());

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(m_program);

        throw std::runtime_error("Shader link failed: " + std::string(infoLog.data()));
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
    if (m_program != 0)
    {
        glDeleteProgram(m_program);
    }
}

void Shader::Use() const
{
    glUseProgram(m_program);
}

GLuint Shader::GetProgram() const
{
    return m_program;
}

void Shader::SetBool(const std::string& name, bool value) const
{
    glUniform1i(GetUniformLocation(name), value ? 1 : 0);
}

void Shader::SetInt(const std::string& name, int value) const
{
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

std::string Shader::ReadTextFile(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + path.u8string());
    }

    std::ostringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

GLuint Shader::CompileStage(GLenum shaderType, const std::string& source, const std::string& label)
{
    const GLuint shader = glCreateShader(shaderType);
    const char* sourcePtr = source.c_str();
    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint infoLogLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<GLchar> infoLog(static_cast<std::size_t>(infoLogLength));
        glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog.data());

        glDeleteShader(shader);
        throw std::runtime_error("Shader compile failed (" + label + "): " + std::string(infoLog.data()));
    }

    return shader;
}

GLint Shader::GetUniformLocation(const std::string& name) const
{
    return glGetUniformLocation(m_program, name.c_str());
}
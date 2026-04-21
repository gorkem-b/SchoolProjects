#include "Graphics/Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

namespace {
std::string ReadTextFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        return {};
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int CompileShader(unsigned int type, const std::string& source)
{
    const char* sourcePtr = source.c_str();
    const unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        int logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(static_cast<std::size_t>(logLength));
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());
        std::cerr << "Shader compilation failed: " << log.data() << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}
}

Shader::~Shader()
{
    if (m_program != 0)
    {
        glDeleteProgram(m_program);
        m_program = 0;
    }
}

bool Shader::LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath)
{
    const std::string vertexSource = ReadTextFile(vertexPath);
    const std::string fragmentSource = ReadTextFile(fragmentPath);

    if (vertexSource.empty() || fragmentSource.empty())
    {
        std::cerr << "Failed to read shader files: " << vertexPath << " or " << fragmentPath << std::endl;
        return false;
    }

    const unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    const unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    if (vertexShader == 0 || fragmentShader == 0)
    {
        if (vertexShader != 0)
        {
            glDeleteShader(vertexShader);
        }

        if (fragmentShader != 0)
        {
            glDeleteShader(fragmentShader);
        }

        return false;
    }

    m_program = glCreateProgram();
    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);
    glLinkProgram(m_program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    int success = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        int logLength = 0;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(static_cast<std::size_t>(logLength));
        glGetProgramInfoLog(m_program, logLength, nullptr, log.data());
        std::cerr << "Shader link failed: " << log.data() << std::endl;
        glDeleteProgram(m_program);
        m_program = 0;
        return false;
    }

    return true;
}

void Shader::Use() const
{
    glUseProgram(m_program);
}

bool Shader::IsValid() const
{
    return m_program != 0;
}

void Shader::SetInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(m_program, name.c_str()), value);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(m_program, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
#include "Shader.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <array>

static GLenum ShaderTypeFromString(const std::string& type)
{
    if (type == "vertex") return GL_VERTEX_SHADER;
    if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;
    std::cout << "Unknown shader type: " << type << "\n";
    return 0;
}

Shader::Shader(const std::string& filepath)
{
    std::string shaderSource = ReadFile(filepath);
    auto shaderSources = PreProcess(shaderSource);
    Compile(shaderSources);

    // Extreure el nom del fitxer
    auto lastSlash = filepath.find_last_of("/\\");
    lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
    auto lastDot = filepath.rfind('.');
    auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
    m_Name = filepath.substr(lastSlash, count);
}

Shader::Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    : m_Name(name)
{
    std::unordered_map<GLenum, std::string> shaderSources;
    shaderSources[GL_VERTEX_SHADER] = vertexSrc;
    shaderSources[GL_FRAGMENT_SHADER] = fragmentSrc;
    Compile(shaderSources);
}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}

std::string Shader::ReadFile(const std::string& filepath)
{
    std::string result;
    std::ifstream in(filepath, std::ios::in | std::ios::binary);
    if (in)
    {
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        if (size != -1)
        {
            result.resize(size);
            in.seekg(0, std::ios::beg);
            in.read(&result[0], size);
        }
        else
        {
            std::cout << "Could not read from file '" << filepath << "'\n";
        }
    }
    else
    {
        std::cout << "Could not open file '" << filepath << "'\n";
    }
    return result;
}

std::unordered_map<GLenum, std::string> Shader::PreProcess(const std::string& source)
{
    std::unordered_map<GLenum, std::string> shaderSources;

    const char* typeToken = "#type";
    size_t typeTokenLength = strlen(typeToken);
    size_t pos = source.find(typeToken, 0);

    while (pos != std::string::npos)
    {
        size_t eol = source.find_first_of("\r\n", pos);
        if (eol == std::string::npos) { std::cout << "Syntax error\n"; break; }

        size_t begin = pos + typeTokenLength + 1;
        std::string type = source.substr(begin, eol - begin);

        size_t nextLinePos = source.find_first_not_of("\r\n", eol);
        if (nextLinePos == std::string::npos) { std::cout << "Syntax error\n"; break; }

        pos = source.find(typeToken, nextLinePos);

        shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
    }

    return shaderSources;
}

void Shader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
{
    GLuint program = glCreateProgram();
    std::vector<GLuint> glShaderIDs;
    glShaderIDs.reserve(shaderSources.size());

    for (auto& kv : shaderSources)
    {
        GLenum type = kv.first;
        const std::string& source = kv.second;

        GLuint shader = glCreateShader(type);
        const GLchar* src = source.c_str();
        glShaderSource(shader, 1, &src, 0);
        glCompileShader(shader);

        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

            glDeleteShader(shader);
            std::cout << "Shader compilation failure!\n" << infoLog.data() << "\n";
            break;
        }

        glAttachShader(program, shader);
        glShaderIDs.push_back(shader);
    }

    m_RendererID = program;
    glLinkProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        glDeleteProgram(program);
        for (auto id : glShaderIDs) glDeleteShader(id);

        std::cout << "Shader link failure!\n" << infoLog.data() << "\n";
        return;
    }

    for (auto id : glShaderIDs)
    {
        glDetachShader(program, id);
        glDeleteShader(id); // Ara sí que els podem esborrar un cop linkats
    }
}

void Shader::Bind() const
{
    glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::SetInt(const std::string& name, int value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1i(location, value);
}

void Shader::SetIntArray(const std::string& name, int* values, uint32_t count)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1iv(location, count, values);
}

void Shader::SetFloat(const std::string& name, float value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1f(location, value);
}

void Shader::SetFloat2(const std::string& name, const glm::vec2& value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform2f(location, value.x, value.y);
}

void Shader::SetFloat3(const std::string& name, const glm::vec3& value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform3f(location, value.x, value.y, value.z);
}

void Shader::SetFloat4(const std::string& name, const glm::vec4& value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}
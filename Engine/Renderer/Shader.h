#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

// Falsificació de GLenum per no incloure glad al header
typedef unsigned int GLenum;

class Shader
{
public:
    // Carrega des d'un sol arxiu .glsl (que conté #type vertex i #type fragment)
    Shader(const std::string& filepath);

    // Opcional: per passar el codi font directament en comptes de llegir arxius
    Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    // Utilitats per enviar dades uniformes (Colors, Matrius de Càmera, etc.)
    void SetInt(const std::string& name, int value);
    void SetIntArray(const std::string& name, int* values, uint32_t count);
    void SetFloat(const std::string& name, float value);
    void SetFloat2(const std::string& name, const glm::vec2& value);
    void SetFloat3(const std::string& name, const glm::vec3& value);
    void SetFloat4(const std::string& name, const glm::vec4& value);
    void SetMat4(const std::string& name, const glm::mat4& value);

    const std::string& GetName() const { return m_Name; }

private:
    std::string ReadFile(const std::string& filepath);
    std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
    void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

private:
    uint32_t m_RendererID;
    std::string m_Name;
};
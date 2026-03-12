#pragma once
#include <string>
#include <glad/glad.h>

enum class ImageFormat
{
    None = 0,
    R8,
    RGB8,
    RGBA8,
    RGBA32F
};

struct TextureSpecification
{
    uint32_t Width = 1;
    uint32_t Height = 1;
    ImageFormat Format = ImageFormat::RGBA8;
    bool GenerateMips = true;
};

class Texture2D
{
public:
    Texture2D(const TextureSpecification& specification);
    Texture2D(const std::string& path);
    ~Texture2D();

    const TextureSpecification& GetSpecification() const { return m_Specification; }

    void Bind(uint32_t slot = 0) const;
    void Unbind() const;

    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }
    uint32_t GetRendererID() const { return m_RendererID; }
    const std::string& GetPath() const { return m_Path; }

    void SetData(void* data, uint32_t size);

    bool operator==(const Texture2D& other) const
    {
        return m_RendererID == other.m_RendererID;
    }

private:
    TextureSpecification m_Specification;
    std::string m_Path;
    uint32_t m_Width = 0;
    uint32_t m_Height = 0;
    uint32_t m_RendererID = 0;
    GLenum m_InternalFormat = 0;
    GLenum m_DataFormat = 0;
};
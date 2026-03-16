#include "SubTexture2D.h"

SubTexture2D::SubTexture2D(const std::shared_ptr<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max)
    : m_Texture(texture)
{
    m_TexCoords[0] = { min.x, min.y };
    m_TexCoords[1] = { max.x, min.y };
    m_TexCoords[2] = { max.x, max.y };
    m_TexCoords[3] = { min.x, max.y };
}

std::shared_ptr<SubTexture2D> SubTexture2D::CreateFromPixelCoords(const std::shared_ptr<Texture2D>& texture, float x, float y, float width, float height)
{
    float texWidth = (float)texture->GetWidth();
    float texHeight = (float)texture->GetHeight();

    float y_GL = texHeight - y - height;

    glm::vec2 min = {
        x / texWidth,
        y_GL / texHeight
    };

    glm::vec2 max = {
        (x + width) / texWidth,
        (y_GL + height) / texHeight
    };

    return std::make_shared<SubTexture2D>(texture, min, max);
}

std::shared_ptr<SubTexture2D> SubTexture2D::CreateFromPixelCoords(const std::shared_ptr<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize)
{
    return CreateFromPixelCoords(texture, coords.x, coords.y, cellSize.x, cellSize.y);
}
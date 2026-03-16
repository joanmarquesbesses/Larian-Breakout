#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "Texture2D.h"

class SubTexture2D
{
public:
    SubTexture2D(const std::shared_ptr<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);

    const std::shared_ptr<Texture2D>& GetTexture() const { return m_Texture; }
    const glm::vec2* GetTexCoords() const { return m_TexCoords; }

    static std::shared_ptr<SubTexture2D> CreateFromCoords(const std::shared_ptr<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize = { 1.0f, 1.0f })
    {
        glm::vec2 min = { (coords.x * cellSize.x) / texture->GetWidth(), (coords.y * cellSize.y) / texture->GetHeight() };
        glm::vec2 max = { ((coords.x + spriteSize.x) * cellSize.x) / texture->GetWidth(), ((coords.y + spriteSize.y) * cellSize.y) / texture->GetHeight() };

        // CORREGIT: Es fa servir std::make_shared en comptes del CreateRef o la sintaxi trencada
        return std::make_shared<SubTexture2D>(texture, min, max);
    }

    static std::shared_ptr<SubTexture2D> CreateFromPixelCoords(const std::shared_ptr<Texture2D>& texture, float x, float y, float width, float height);
    static std::shared_ptr<SubTexture2D> CreateFromPixelCoords(const std::shared_ptr<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize);

private:
    std::shared_ptr<Texture2D> m_Texture;
    glm::vec2 m_TexCoords[4];
};
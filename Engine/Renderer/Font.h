#pragma once
#include <string>
#include <memory>
#include "Texture2D.h"
#include "stb_truetype.h"

class Font {
public:
    Font(const std::string& path, float size = 64.0f);

    std::shared_ptr<Texture2D> GetAtlas() const { return m_Atlas; }
    const stbtt_bakedchar* GetBakedChars() const { return m_CData; }

private:
    std::shared_ptr<Texture2D> m_Atlas;
    stbtt_bakedchar m_CData[96];
};
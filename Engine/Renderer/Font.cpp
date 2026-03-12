#include "Font.h"
#include <fstream>
#include <vector>
#include <iostream>

Font::Font(const std::string& path, float size) {

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cout << "Error: No s'ha pogut carregar la font a " << path << "\n";
        return;
    }
    std::streamsize ttf_size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<unsigned char> ttf_buffer(ttf_size);
    if (!file.read((char*)ttf_buffer.data(), ttf_size)) return;

    const int atlasSize = 1024;
    std::vector<unsigned char> temp_bitmap(atlasSize * atlasSize);

    stbtt_BakeFontBitmap(ttf_buffer.data(), 0, size, temp_bitmap.data(), atlasSize, atlasSize, 32, 96, m_CData);

    std::vector<unsigned char> rgba_bitmap(atlasSize * atlasSize * 4);

    for (int i = 0; i < atlasSize * atlasSize; i++) {
        rgba_bitmap[i * 4 + 0] = 255; // Red
        rgba_bitmap[i * 4 + 1] = 255; // Green
        rgba_bitmap[i * 4 + 2] = 255; // Blue
        rgba_bitmap[i * 4 + 3] = temp_bitmap[i]; // Alpha 
    }

    TextureSpecification spec;
    spec.Width = atlasSize;
    spec.Height = atlasSize;
    spec.Format = ImageFormat::RGBA8;

    m_Atlas = std::make_shared<Texture2D>(spec);
    m_Atlas->SetData(rgba_bitmap.data(), atlasSize * atlasSize * 4);
}
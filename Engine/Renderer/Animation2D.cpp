#include "Animation2D.h"

Animation2D::Animation2D(const std::vector<std::shared_ptr<SubTexture2D>>& frames, float frameTime)
    : m_Frames(frames), m_FrameTime(frameTime)
{
}

std::shared_ptr<SubTexture2D> Animation2D::GetFrame(uint32_t index) const
{
    if (index >= m_Frames.size()) return nullptr;
    return m_Frames[index];
}

std::shared_ptr<Animation2D> Animation2D::CreateFromAtlas(std::shared_ptr<Texture2D> atlas, const glm::vec2& cellSize, const glm::vec2& startCoords, int count, int framesPerRow, float frameTime)
{
    std::vector<std::shared_ptr<SubTexture2D>> frames;

    int validFramesPerRow = framesPerRow > 0 ? framesPerRow : 1;

    for (int i = 0; i < count; i++)
    {
        int relativeCol = i % validFramesPerRow;
        int relativeRow = i / validFramesPerRow;

        float pixelX = startCoords.x + (relativeCol * cellSize.x);
        float pixelY = startCoords.y + (relativeRow * cellSize.y);

        auto subtexture = SubTexture2D::CreateFromPixelCoords(atlas, pixelX, pixelY, cellSize.x, cellSize.y);
        frames.push_back(subtexture);
    }

    return std::make_shared<Animation2D>(frames, frameTime);
}
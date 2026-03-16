#pragma once

#include "SubTexture2D.h"
#include <vector>
#include <memory>

class Animation2D
{
public:
    Animation2D() = default;
    Animation2D(const std::vector<std::shared_ptr<SubTexture2D>>& frames, float frameTime);

    std::shared_ptr<SubTexture2D> GetFrame(uint32_t index) const;
    uint32_t GetFrameCount() const { return (uint32_t)m_Frames.size(); }
    float GetFrameTime() const { return m_FrameTime; }

    static std::shared_ptr<Animation2D> CreateFromAtlas(std::shared_ptr<Texture2D> atlas, const glm::vec2& cellSize, const glm::vec2& startCoords, int count, int framesPerRow, float frameTime);

private:
    std::vector<std::shared_ptr<SubTexture2D>> m_Frames;
    float m_FrameTime = 0.1f; // Temps per frame (ex: 0.1s = 10fps)
};
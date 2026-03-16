#pragma once
#include<glm/glm.hpp>

#include "Renderer/Subtexture2D.h"

class Paddle {
private:
    glm::vec2 m_Position = { 0.0f, -0.8f };
    glm::vec2 m_Size = { 0.6f, 0.1f };

    float m_Speed = 2.0f;

    std::shared_ptr<SubTexture2D> m_Texture;

public:
    Paddle() = default;

    Paddle(glm::vec2 startPos, glm::vec2 size, float speed)
        : m_Position(startPos), m_Size(size), m_Speed(speed) {
    }

    void MoveLeft(float dt) { m_Position.x -= m_Speed * dt; }
    void MoveRight(float dt) { m_Position.x += m_Speed * dt; }

    glm::vec2 GetPosition() const { return m_Position; }
    void SetPosition(const glm::vec2& position) { m_Position = position; }

    glm::vec2 GetSize() const { return m_Size; }
    void SetSize(const glm::vec2& position) { m_Size = position; }

    void SetSpeed(float speed) { m_Speed = speed; }

	std::shared_ptr<SubTexture2D> GetTexture() const { return m_Texture; }
	void SetTexture(const std::shared_ptr<SubTexture2D>& texture) { m_Texture = texture; }
};

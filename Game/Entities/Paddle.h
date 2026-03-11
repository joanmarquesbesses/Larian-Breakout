#pragma once
#include<glm/glm.hpp>

class Paddle {
private:
    glm::vec2 m_Position;
    glm::vec2 m_Size = { 0.6f, 0.1f };

    float m_Speed = 2.0f;

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
};

#pragma once
#include <glm/glm.hpp>

class Ball {
private:
    glm::vec2 m_Position;
    glm::vec2 m_Velocity;

    float m_Radius;

public:

    Ball() = default;

    Ball(glm::vec2 startPos, float radius) 
        : m_Position(startPos), m_Velocity(0.0f, 0.0f), m_Radius(radius) {
    }

    void Move(float deltaTime) {
        m_Position += m_Velocity * deltaTime;
    }

    void Launch(glm::vec2 initialVelocity) {
        m_Velocity = initialVelocity;
    }

    glm::vec2 GetPosition() const { return m_Position; }
    void SetPosition(const glm::vec2& position) { m_Position = position; }

    glm::vec2 GetVelocity() const { return m_Velocity; }
    void SetVelocity(const glm::vec2& velocity) { m_Velocity = velocity; }

    float GetSize() const { return m_Radius; }
    void SetSize(float radius) { m_Radius = radius; }
};
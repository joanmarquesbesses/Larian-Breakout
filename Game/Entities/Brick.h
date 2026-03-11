#pragma once
#include <optional>
#include <glm/glm.hpp>

enum class PowerUpType {
    ExtraLife,
    ExpandPaddle,
    MultiBall
};

class Brick {
private:
    glm::vec2 m_Position;
    glm::vec2 m_Size;

    int m_Health;
    glm::vec4 m_Color;

    std::optional<PowerUpType> m_Reward;

public:
    Brick(glm::vec2 pos, glm::vec2 size, int health, glm::vec4 color, std::optional<PowerUpType> powerUp = std::nullopt)
        : m_Position(pos), m_Size(size), m_Health(health), m_Color(color), m_Reward(powerUp) {
    }

    glm::vec2 GetPosition() const { return m_Position; }
    void SetPosition(const glm::vec2& position) { m_Position = position; }

    glm::vec2 GetSize() const { return m_Size; }
    void SetSize(const glm::vec2& size) { m_Size = size; }

    const glm::vec4& GetColor() const { return m_Color; }

    void TakeDamage() {
        if (m_Health > 0) m_Health--;
    }

    bool IsDestroyed() const { return m_Health <= 0; }

    std::optional<PowerUpType> TryExtractPowerUp() {
        auto reward = m_Reward;
        m_Reward = std::nullopt;
        return reward;
    }
};
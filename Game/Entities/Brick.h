#pragma once
#include <optional>
#include <glm/glm.hpp>

#include "Renderer/Animation2D.h"

class Brick {
private:
    glm::vec2 m_Position;
    glm::vec2 m_Size;

    int m_Health;
    int m_MaxHealth = 1;
    glm::vec4 m_Color;

    std::shared_ptr<Animation2D> m_DestroyAnim = nullptr;
    std::shared_ptr<SubTexture2D> m_IdleTexture = nullptr;

    float m_AnimTime = 0.0f;
    bool m_IsDying = false;      
    bool m_IsDestroyed = false;  

    bool m_IsExplosive = false;

public:
    Brick(glm::vec2 pos, glm::vec2 size, int health = 1, glm::vec4 color = glm::vec4{ 1.0f }, std::optional<PowerUpType> powerUp = std::nullopt)
        : m_Position(pos), m_Size(size), m_Health(health), m_Color(color) {
    }

    const glm::vec2& GetPosition() const { return m_Position; }
    void SetPosition(const glm::vec2& position) { m_Position = position; }

    const glm::vec2& GetSize() const { return m_Size; }
    void SetSize(const glm::vec2& size) { m_Size = size; }

    const glm::vec4& GetColor() const { return m_Color; }
    void SetColor(const glm::vec4& color) { m_Color = color; }

    void SetHealth(int health) { m_Health = health; }
    int GetHealth() { return m_Health; }

    int GetMaxHealth() const { return m_MaxHealth; }
    void SetMaxHealth(int max) { m_MaxHealth = max; }

    void TakeDamage() {
        if (m_Health > 0) m_Health--;
    }

    void SetIdleTexture(std::shared_ptr<SubTexture2D> tex) { m_IdleTexture = tex; }
    std::shared_ptr<SubTexture2D> GetIdleTexture() const { return m_IdleTexture; }

    void SetDestroyAnimation(std::shared_ptr<Animation2D> anim) { m_DestroyAnim = anim; }
    std::shared_ptr<Animation2D> GetDestroyAnim() const { return m_DestroyAnim; }

    void StartDying() { m_IsDying = true; }
    bool IsDying() const { return m_IsDying; }

    void SetDestroyed() { m_IsDestroyed = true; }
    bool IsDestroyed() const { return m_IsDestroyed; }

    void AddAnimTime(float dt) { m_AnimTime += dt; }
    float GetAnimTime() const { return m_AnimTime; }

    bool IsExplosive() const { return m_IsExplosive; }
    void SetExplosive(bool explosive) { m_IsExplosive = explosive; }
};
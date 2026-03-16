#pragma once

#include <glm/glm.hpp>
#include "Renderer/Texture2D.h"

enum class PowerUpType {
    ExtraLife,   
    Enlarge,     
    Multiball    
};

class PowerUp {
private:
    glm::vec2 m_Position;
    glm::vec2 m_Size;
    glm::vec2 m_Velocity;
    glm::vec4 m_Color;

    PowerUpType m_Type;
    bool m_IsDestroyed = false;

	std::shared_ptr<Texture2D> m_Texture;

public:
    PowerUp(glm::vec2 position, glm::vec2 size, PowerUpType type)
        : m_Position(position), m_Size(size), m_Type(type) {

        m_Velocity = { 0.0f, -0.6f };

        switch (m_Type) {
        case PowerUpType::ExtraLife:
            m_Color = { 0.9f, 0.2f, 0.2f, 1.0f }; // Red
            break;
        case PowerUpType::Enlarge:
            m_Color = { 0.2f, 0.4f, 0.9f, 1.0f }; // Blue
            break;
        case PowerUpType::Multiball:
            m_Color = { 0.2f, 0.9f, 0.9f, 1.0f }; // Cian
            break;
        }
    }

    const glm::vec2& GetPosition() const { return m_Position; }
    const glm::vec2& GetSize() const { return m_Size; }
    const glm::vec4& GetColor() const { return m_Color; }
    PowerUpType GetType() const { return m_Type; }
    bool IsDestroyed() const { return m_IsDestroyed; }

    void Destroy() { m_IsDestroyed = true; }

    void Move(float dt) {
        m_Position += m_Velocity * dt;
    }

	void SetTexture(const std::shared_ptr<Texture2D>& texture) { m_Texture = texture; }
	const std::shared_ptr<Texture2D>& GetTexture() const { return m_Texture; }
};

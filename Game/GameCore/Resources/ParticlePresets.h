#pragma once
#include "GameCore/Systems/ParticleSystem.h" 
#include <glm/glm.hpp>

namespace ParticlePresets {

    // Brick explosion: bursts of particles when a brick gets destroyed.
    inline ParticleProps GetBrickExplosion(const glm::vec2& position, const glm::vec4& color) {
        ParticleProps props;
        props.Position = position;
        props.Velocity = { 0.0f, 0.0f };
        props.VelocityVariation = { 1.5f, 1.5f };
        props.SizeBegin = 0.04f;
        props.SizeEnd = 0.0f;
        props.SizeVariation = 0.01f;
        props.ColorBegin = color;
        props.ColorEnd = { color.r, color.g, color.b, 0.0f };
        props.LifeTime = 0.5f;
        return props;
    }

    // Power-Up trail: a small trail of particles following the power-up as it falls.
    inline ParticleProps GetPowerUpTrail(const glm::vec2& position, const glm::vec4& color) {
        ParticleProps props;

        float offsetX = ((rand() % 100) / 100.0f - 0.5f) * 0.1f;
        float offsetY = ((rand() % 100) / 100.0f - 0.5f) * 0.1f;
        props.Position = { position.x + offsetX, position.y + offsetY };
        props.Velocity = { 0.0f, 0.1f };
        props.VelocityVariation = { 0.4f, 0.4f };
        props.SizeBegin = 0.02f;
        props.SizeEnd = 0.0f;
        props.SizeVariation = 0.01f;
        props.ColorBegin = color;
        props.ColorEnd = { color.r, color.g, color.b, 0.0f }; 
        props.LifeTime = 0.15f;

        return props;
    }

    // Deadzone hit: burst of particles shooting upwards as if bouncing off the bottom bounds.
    inline ParticleProps GetBallDeath(const glm::vec2& position) {
        ParticleProps props;
        props.Position = position;
        props.Velocity = { 0.0f, 1.0f }; // Shoot upwards (bouncing from bottom)
        props.VelocityVariation = { 2.0f, 0.5f };
        props.SizeBegin = 0.05f;
        props.SizeEnd = 0.0f;
        props.SizeVariation = 0.02f;
        props.ColorBegin = { 1.0f, 0.1f, 0.1f, 1.0f }; // Danger Red
        props.ColorEnd = { 1.0f, 0.5f, 0.0f, 0.0f }; // Fade to orange and transparent
        props.LifeTime = 0.6f;
        return props;
    }

    // Background stars: small particles that slowly fall down the screen to add depth to the background.
    inline ParticleProps GetStar() {
        ParticleProps props;
        float randomX = ((rand() % 500) / 100.0f) - 3.0f;

        props.Position = { randomX, 1.2f };

        props.Velocity = { 0.05f, -0.1f };
        props.VelocityVariation = { 0.02f, 0.02f };

        props.SizeBegin = 0.008f;
        props.SizeEnd = 0.008f;
        props.SizeVariation = 0.003f;

        props.ColorBegin = { 1.0f, 1.0f, 1.0f, 0.6f };
        props.ColorEnd = { 1.0f, 1.0f, 1.0f, 0.0f };

        props.LifeTime = 25.0f;
        return props;
    }
}
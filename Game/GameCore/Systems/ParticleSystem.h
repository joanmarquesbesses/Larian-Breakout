#pragma once

#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp> 
#include "Renderer/Renderer.h"
#include "Utils/Random.h"

struct ParticleProps {
    glm::vec2 Position;
    glm::vec2 Velocity, VelocityVariation;
    glm::vec4 ColorBegin, ColorEnd;
    float SizeBegin, SizeEnd, SizeVariation;
    float LifeTime = 1.0f;
};

class ParticleSystem {
private:
    struct Particle {
        glm::vec2 Position;
        glm::vec2 Velocity;
        glm::vec4 ColorBegin, ColorEnd;
        float Rotation = 0.0f;
        float SizeBegin, SizeEnd;
        float LifeTime = 1.0f;
        float LifeRemaining = 0.0f;
        bool Active = false;
    };

    std::vector<Particle> m_ParticlePool;
    uint32_t m_PoolIndex = 999;

public:
    ParticleSystem(uint32_t maxParticles = 1000) {
        m_ParticlePool.resize(maxParticles);
        m_PoolIndex = maxParticles - 1;
    }

    void OnUpdate(float ts) {
        for (auto& particle : m_ParticlePool) {
            if (!particle.Active) continue;

            if (particle.LifeRemaining <= 0.0f) {
                particle.Active = false;
                continue;
            }

            particle.LifeRemaining -= ts;
            particle.Position += particle.Velocity * ts;
            // particle.Rotation += 2.0f * ts; // Si vols que girin
        }
    }

    void OnRender() {
        for (auto& particle : m_ParticlePool) {
            if (!particle.Active) continue;

            // Fade out de vida (de 1.0 a 0.0)
            float life = particle.LifeRemaining / particle.LifeTime;

            glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);
            float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);

            // Si el teu motor té DrawRotatedQuad, fes-lo servir. Si no, amb DrawQuad per a quadradets petits ja fa l'efecte!
            Renderer::DrawQuad(particle.Position, { size, size }, color);
        }
    }

    void Emit(const ParticleProps& particleProps) {
        Particle& particle = m_ParticlePool[m_PoolIndex];
        particle.Active = true;
        particle.Position = particleProps.Position;

        particle.Velocity = particleProps.Velocity;
        particle.Velocity.x += particleProps.VelocityVariation.x * (Random::Range(-0.5f, 0.5f));
        particle.Velocity.y += particleProps.VelocityVariation.y * (Random::Range(-0.5f, 0.5f));

        particle.ColorBegin = particleProps.ColorBegin;
        particle.ColorEnd = particleProps.ColorEnd;

        particle.LifeTime = particleProps.LifeTime;
        particle.LifeRemaining = particleProps.LifeTime;

        particle.SizeBegin = particleProps.SizeBegin + particleProps.SizeVariation * (Random::Range(-0.5f, 0.5f));
        particle.SizeEnd = particleProps.SizeEnd;

        m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
    }
};
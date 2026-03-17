#pragma once
#include "IGameState.h"

#include "Renderer/OrthographicCamera.h"
#include "Renderer/Renderer.h"
#include "Audio/AudioEngine.h"
#include "Assets/ResourceManager.h"
#include "Audio/AudioClip.h"

#include "../Controllers/PlayerController.h"
#include "GameCore/Systems/ParticleSystem.h"
#include "GameCore/Resources/ParticlePresets.h"

class TitleScreenState : public IGameState {
private:
    OrthographicCamera m_Camera;

    std::shared_ptr<Font> m_TextFont;
    std::shared_ptr<Texture2D> m_NebulaTexture;

    float m_TitleScale = 0.003f;
    float m_SubtitleScale = 0.001f;

    std::string m_TitleText = "LARIAN BREAKOUT";
    std::string m_SubtitleText = "Press SPACE to Start";

    float m_Time = 0.0f;
    bool m_IsTransitioning = false;

    std::shared_ptr<AudioClip> m_ConfirmSFX;

    // Background scrolling offset
    float m_NebulaOffsetV = 0.0f;
    float m_NebulaOffsetU = 0.0f;
    ParticleSystem m_BgParticleSystem;

public:
    TitleScreenState() : m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) {}

    void OnEnter() override {
        m_TextFont = ResourceManager::Get<Font>("Assets/Font/BitFont.ttf");
        m_ConfirmSFX = ResourceManager::Get<AudioClip>("Assets/SFX/Accept.mp3");
        m_NebulaTexture = ResourceManager::Get<Texture2D>("Assets/Textures/nebula.png");
        AudioEngine::PlayMusic("Assets/Music/TitleScreen.mp3", true);

        // Pre-warm the background particle system with some initial stars
        for (int i = 0; i < 40; i++) {
            ParticleProps initialStar = ParticlePresets::GetStar();
            initialStar.Position.y = Random::Range(-1.0f, 1.0f);
            initialStar.LifeTime = Random::Range(10.0f, 25.0f);
            m_BgParticleSystem.Emit(initialStar);
        }

        m_Time = 0.0f;
        m_IsTransitioning = false;
    }

    void OnUpdate(Timestep ts) override {
        float dt = ts.GetSeconds();
        m_Time += dt;

        // Scroll background texture
        m_NebulaOffsetV += dt * 0.003f;
        m_NebulaOffsetU += dt * 0.002f;

        // Randomly emit new background stars
        if (Random::Range(0,100) < 1) {
            m_BgParticleSystem.Emit(ParticlePresets::GetStar());
        }
        m_BgParticleSystem.OnUpdate(dt);

        // Wait 1.5 seconds before allowing input, then check for start action
        if (m_Time > 1.5f && !m_IsTransitioning && PlayerController::ConsumeIfPressed(PlayerAction::Fire)) {
            m_IsTransitioning = true;
            if (m_RequestStateChange) {
                m_RequestStateChange(GameStateType::MainMenu);
                if (m_ConfirmSFX) AudioEngine::Play(m_ConfirmSFX->GetPath());
            }
        }
    }

    void OnRender() override {
        Renderer::SetClearColor({ 0.05f, 0.05f, 0.1f, 1.0f });
        Renderer::Clear();
        Renderer::BeginScene(m_Camera.GetViewProjectionMatrix());

        // Render scrolling background nebula
        if (m_NebulaTexture) {
            glm::mat4 bgTransform = glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, 0.0f })
                * glm::scale(glm::mat4(1.0f), { 4.0f, 4.0f, 1.0f });

            glm::vec2 nebulaUVs[4] = {
                { 0.0f + m_NebulaOffsetU, 0.0f + m_NebulaOffsetV },
                { 1.0f + m_NebulaOffsetU, 0.0f + m_NebulaOffsetV },
                { 1.0f + m_NebulaOffsetU, 1.0f + m_NebulaOffsetV },
                { 0.0f + m_NebulaOffsetU, 1.0f + m_NebulaOffsetV }
            };
            Renderer::DrawQuad(bgTransform, m_NebulaTexture, nebulaUVs);
        }

        m_BgParticleSystem.OnRender();

        // Title fade-in logic
        float titleAlpha = std::clamp(m_Time - 1.0f, 0.0f, 1.0f);
        if (titleAlpha > 0.0f) {
            float titleWidth = Renderer::GetTextWidth(m_TitleText, m_TitleScale, m_TextFont);
            Renderer::DrawString(m_TitleText, { 0.0f - (titleWidth / 2), 0.3f }, m_TitleScale, { 0.8f, 0.2f, 0.2f, titleAlpha }, m_TextFont);
        }

        // Subtitle ("Press SPACE") fade-in and pulsing logic
        if (m_Time > 2.0f) {
            float subtitleWidth = Renderer::GetTextWidth(m_SubtitleText, m_SubtitleScale, m_TextFont);

            float fadeInAlpha = std::clamp((m_Time - 2.0f) / 2.0f, 0.0f, 1.0f);

            // Sine wave for pulsing opacity between 0.3 and 1.0
            float blinkAlpha = (std::sin((m_Time - 2.0f) * 4.0f) + 1.0f) / 2.0f;
            blinkAlpha = std::clamp(blinkAlpha, 0.3f, 1.0f);

            float finalAlpha = fadeInAlpha * blinkAlpha;

            Renderer::DrawString(m_SubtitleText, { 0.0f - (subtitleWidth / 2), 0.0f }, m_SubtitleScale, { 1.0f, 1.0f, 1.0f, finalAlpha }, m_TextFont);
        }

        Renderer::EndScene();
    }
  
};
#pragma once
#include "IGameState.h"

#include "Renderer/OrthographicCamera.h"
#include "Renderer/Renderer.h"
#include "Assets/ResourceManager.h"

#include "GameCore/GameSession.h"
#include "../Controllers/PlayerController.h"
#include "../Services/ScoreSerializer.h"

class GameOverState : public IGameState {
private:
    GameSession* m_Session;

    OrthographicCamera m_Camera;

    std::shared_ptr<Font> m_TextFont;

    float m_TitleScale = 0.003f;
    float m_FinalScoreScale = 0.0012f;
    float m_HighScoreScale = 0.0012f;
    float m_SubtitleScale = 0.0005f;

    std::string m_TitleText = "GAME OVER";
    std::string m_FinalScoreText = "SCORE: ";
    std::string m_HighScoreText = "HIGH SCORE: ";
    std::string m_SubtitleText = "PRESS SPACE TO MAIN MENU";

    float m_Time = 0.0f;
    bool m_NewHighScore = false;
    bool m_IsTransitioning = false;

    std::shared_ptr<Texture2D> m_Nebula;
    float m_NebulaOffsetV = 0.0f;
    float m_NebulaOffsetU = 0.0f;
    ParticleSystem m_BgParticleSystem;

public:
    GameOverState(GameSession* session) : m_Session(session), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) {}

    void OnEnter() override {
        m_TextFont = ResourceManager::Get<Font>("Assets/Font/BitFont.ttf");
        m_Nebula = ResourceManager::Get<Texture2D>("Assets/Textures/nebula.png");
        m_NewHighScore = ScoreSerializer::SaveHighScore(m_Session->GetPlayer().GetScore());
        AudioEngine::StopMusic();
        AudioEngine::PlayMusic("Assets/Music/GameOver.mp3", true);

        m_Time = 0.0f;
        m_IsTransitioning = false;

        for (int i = 0; i < 40; i++) {
            ParticleProps initialStar = ParticlePresets::GetStar();
            float randomY = ((rand() % 200) / 100.0f) - 1.0f;
            initialStar.Position.y = randomY;
            initialStar.LifeTime = 10.0f + ((rand() % 1500) / 100.0f);
            m_BgParticleSystem.Emit(initialStar);
        }
    }

    void OnUpdate(Timestep ts) override {
        float dt = ts.GetSeconds();
        m_Time += dt;

        m_NebulaOffsetV += dt * 0.003f;
        m_NebulaOffsetU += dt * 0.002f;

        if ((rand() % 100) < 1) {
            m_BgParticleSystem.Emit(ParticlePresets::GetStar());
        }
        m_BgParticleSystem.OnUpdate(dt);

		if (m_Time < 0.5f) return;

        if (!m_IsTransitioning && PlayerController::ConsumeIfPressed(PlayerAction::Fire)) {
            m_IsTransitioning = true;
            if (m_RequestStateChange) {
                m_RequestStateChange(GameStateType::MainMenu);
            }
        }
    }

    void OnRender() override {
        Renderer::SetClearColor({ 0.15f, 0.05f, 0.05f, 1.0f });
        Renderer::Clear();
        Renderer::BeginScene(m_Camera.GetViewProjectionMatrix());

        if (m_Nebula) {
            glm::mat4 bgTransform = glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, 0.0f })
                * glm::scale(glm::mat4(1.0f), { 4.0f, 4.0f, 1.0f });

            glm::vec2 nebulaUVs[4] = {
                { 0.0f + m_NebulaOffsetU, 0.0f + m_NebulaOffsetV },
                { 1.0f + m_NebulaOffsetU, 0.0f + m_NebulaOffsetV },
                { 1.0f + m_NebulaOffsetU, 1.0f + m_NebulaOffsetV },
                { 0.0f + m_NebulaOffsetU, 1.0f + m_NebulaOffsetV }
            };

            Renderer::DrawQuad(bgTransform, m_Nebula, nebulaUVs, { 0.9f, 0.5f, 0.5f, 1.0f });
        }

        m_BgParticleSystem.OnRender();

        float titleWidth = Renderer::GetTextWidth(m_TitleText, m_TitleScale, m_TextFont);
        Renderer::DrawString(m_TitleText, { 0.0f - (titleWidth / 2), 0.55f }, m_TitleScale, { 0.8f, 0.2f, 0.2f, 1.0f }, m_TextFont);

        if (m_NewHighScore) {
            float newHighScoreWidth = Renderer::GetTextWidth("NEW HIGH SCORE!", m_FinalScoreScale, m_TextFont);
            Renderer::DrawString("NEW HIGH SCORE!", { 0.0f - (newHighScoreWidth / 2), 0.11f }, m_FinalScoreScale, { 0.8f, 0.2f, 0.2f, 1.0f }, m_TextFont);
        }

        m_FinalScoreText = "SCORE: " + std::to_string(m_Session->GetPlayer().GetScore());
        float finalScoreWidth = Renderer::GetTextWidth(m_FinalScoreText, m_FinalScoreScale, m_TextFont);
        Renderer::DrawString(m_FinalScoreText, { 0.0f - (finalScoreWidth / 2), 0.0f }, m_FinalScoreScale, { 0.8f, 0.2f, 0.2f, 1.0f }, m_TextFont);

        m_HighScoreText = "HIGH SCORE: " + std::to_string(m_Session->GetPlayer().GetHighScore());
        float highScoreWidth = Renderer::GetTextWidth(m_HighScoreText, m_HighScoreScale, m_TextFont);
        Renderer::DrawString(m_HighScoreText, { 0.0f - (highScoreWidth / 2), -0.15f }, m_HighScoreScale, { 0.8f, 0.2f, 0.2f, 1.0f }, m_TextFont);

        float subtitleWidth = Renderer::GetTextWidth(m_SubtitleText, m_SubtitleScale, m_TextFont);

        float blinkAlpha = (std::sin(m_Time * 4.0f) + 1.0f) / 2.0f;
        blinkAlpha = std::clamp(blinkAlpha, 0.3f, 1.0f);

        Renderer::DrawString(m_SubtitleText, { 0.0f - (subtitleWidth / 2), -0.5f }, m_SubtitleScale, { 1.0f, 1.0f, 1.0f, blinkAlpha }, m_TextFont);

        Renderer::EndScene();
    }
};
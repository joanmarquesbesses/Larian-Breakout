#pragma once
#include "IGameState.h"

#include "Renderer/OrthographicCamera.h"
#include "Renderer/Renderer.h"
#include "Assets/ResourceManager.h"

#include "GameCore/GameSession.h"
#include "../Controllers/PlayerController.h"
#include "../Services/SerializeScore.h"

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

public:
    GameOverState(GameSession* session) : m_Session(session), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) {}

    void OnEnter() override {
        m_TextFont = ResourceManager::Get<Font>("BitFont.ttf");
        m_NewHighScore = SerializeScore::SaveHighScore(m_Session->GetPlayer().GetScore());
    }

    void OnUpdate(Timestep ts) override {
        m_Time += ts.GetSeconds();

        if (PlayerController::ConsumeIfPressed(PlayerAction::Fire)) {
            if (m_RequestStateChange) {
                m_RequestStateChange(GameStateType::MainMenu);
            }
        }
    }

    void OnRender() override {
        Renderer::SetClearColor({ 0.15f, 0.05f, 0.05f, 1.0f });
        Renderer::Clear();
        Renderer::BeginScene(m_Camera.GetViewProjectionMatrix());

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
        float alpha = std::clamp(std::abs(std::sin(m_Time * 1.0f)) * 1.5f, 0.0f, 1.0f);
        Renderer::DrawString(m_SubtitleText, { 0.0f - (subtitleWidth / 2), -0.5f }, m_SubtitleScale, { 1.0f, 1.0f, 1.0f, alpha }, m_TextFont);

        Renderer::EndScene();
    }
};
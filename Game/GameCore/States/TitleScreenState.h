#pragma once
#include "IGameState.h"

#include "Renderer/OrthographicCamera.h"
#include "Renderer/Renderer.h"
#include "Assets/ResourceManager.h"

#include "../Controllers/PlayerController.h"

class TitleScreenState : public IGameState {
private:
    OrthographicCamera m_Camera;

    std::shared_ptr<Font> m_TextFont;

    float m_TitleScale = 0.0015f;
    float m_SubtitleScale = 0.0005f;

    std::string m_TitleText = "LARIAN BREAKOUT";
    std::string m_SubtitleText = "Press SPACE to Start";

    float m_Time = 0.0f;

public:
    TitleScreenState() : m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) {}

    void OnEnter() override {
        m_TextFont = ResourceManager::Get<Font>("BitFont.ttf");
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
        Renderer::SetClearColor({ 0.05f, 0.05f, 0.1f, 1.0f });
        Renderer::Clear();
        Renderer::BeginScene(m_Camera.GetViewProjectionMatrix());

        float titleWidth = Renderer::GetTextWidth(m_TitleText, m_TitleScale, m_TextFont);
        Renderer::DrawString(m_TitleText, { 0.0f - (titleWidth / 2), 0.3f}, 0.0015f, {0.8f, 0.2f, 0.2f, 1.0f}, m_TextFont);

        float subtitleWidth = Renderer::GetTextWidth(m_SubtitleText, m_SubtitleScale, m_TextFont);
        float alpha = std::clamp(std::abs(std::sin(m_Time * 1.0f)) * 1.5f, 0.0f, 1.0f);
        Renderer::DrawString(m_SubtitleText, { 0.0f - (subtitleWidth / 2), 0.0f }, 0.0005f, { 1.0f, 1.0f, 1.0f, alpha }, m_TextFont);

        Renderer::EndScene();
    }
};
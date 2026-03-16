#pragma once
#include "IGameState.h"

#include "Renderer/OrthographicCamera.h"
#include "Renderer/Renderer.h"
#include "Audio/AudioEngine.h"
#include "Assets/ResourceManager.h"
#include "Audio/AudioClip.h"

#include "../Controllers/PlayerController.h"

class TitleScreenState : public IGameState {
private:
    OrthographicCamera m_Camera;

    std::shared_ptr<Font> m_TextFont;

    float m_TitleScale = 0.003f;
    float m_SubtitleScale = 0.001f;

    std::string m_TitleText = "LARIAN BREAKOUT";
    std::string m_SubtitleText = "Press SPACE to Start";

    float m_Time = 0.0f;

    std::shared_ptr<AudioClip> m_ConfirmSFX;

public:
    TitleScreenState() : m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) {}

    void OnEnter() override {
        m_TextFont = ResourceManager::Get<Font>("BitFont.ttf");
        m_ConfirmSFX = ResourceManager::Get<AudioClip>("accept.mp3");
    }

    void OnUpdate(Timestep ts) override {
        m_Time += ts.GetSeconds();

        if (PlayerController::ConsumeIfPressed(PlayerAction::Fire)) {
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

        float titleWidth = Renderer::GetTextWidth(m_TitleText, m_TitleScale, m_TextFont);
        Renderer::DrawString(m_TitleText, { 0.0f - (titleWidth / 2), 0.3f}, m_TitleScale, {0.8f, 0.2f, 0.2f, 1.0f}, m_TextFont);

        float subtitleWidth = Renderer::GetTextWidth(m_SubtitleText, m_SubtitleScale, m_TextFont);
        float alpha = std::clamp(std::abs(std::sin(m_Time * 1.0f)) * 1.5f, 0.0f, 1.0f);
        Renderer::DrawString(m_SubtitleText, { 0.0f - (subtitleWidth / 2), 0.0f }, m_SubtitleScale, { 1.0f, 1.0f, 1.0f, alpha }, m_TextFont);

        Renderer::EndScene();
    }
};
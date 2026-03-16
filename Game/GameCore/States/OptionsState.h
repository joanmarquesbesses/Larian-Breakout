#pragma once

#include <algorithm>
#include <cmath>

#include "IGameState.h"
#include "Renderer/Renderer.h"
#include "Assets/ResourceManager.h"
#include "Audio/AudioEngine.h" 

class OptionsState : public IGameState {
private:
    std::shared_ptr<Font> m_Font;
    int m_SelectedIndex = 0; // 0 = Volum, 1 = Back
    float m_Volume = 1.0f;

    OrthographicCamera m_Camera;

public:
    OptionsState() : m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) {}

    void OnEnter() override {
        m_Font = ResourceManager::Get<Font>("BitFont.ttf");
        m_Volume = AudioEngine::GetVolume();
        m_Camera = OrthographicCamera(-1.6f, 1.6f, -0.9f, 0.9f);
    }

    void OnUpdate(Timestep ts) override {
        if (PlayerController::ConsumeIfPressed(PlayerAction::Up) || PlayerController::ConsumeIfPressed(PlayerAction::Down)) {
            m_SelectedIndex = (m_SelectedIndex == 0) ? 1 : 0;
        }

        if (m_SelectedIndex == 0) {
            if (PlayerController::ConsumeIfPressed(PlayerAction::Left))  m_Volume -= 0.1f;
            if (PlayerController::ConsumeIfPressed(PlayerAction::Right) || PlayerController::ConsumeIfPressed(PlayerAction::Fire)) m_Volume += 0.1f;

            m_Volume = std::clamp(m_Volume, 0.0f, 1.0f);
            AudioEngine::SetVolume(m_Volume);
        }
        else if (m_SelectedIndex == 1) {
            if (PlayerController::ConsumeIfPressed(PlayerAction::Fire)) {
                if (m_RequestStateChange) m_RequestStateChange(GameStateType::MainMenu);
            }
        }
    }

    void OnRender() override {
        Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        Renderer::Clear();
        Renderer::BeginScene(m_Camera.GetViewProjectionMatrix());

        Renderer::DrawString("OPTIONS", { -0.5f, 0.6f }, 0.002f, { 1.0f, 1.0f, 1.0f, 1.0f }, m_Font);

        std::string volText = "VOLUME: < " + std::to_string((int)std::round(m_Volume * 100.0f)) + "% >";
        float volumeWidth = Renderer::GetTextWidth(volText, 0.0015f, m_Font);
        glm::vec4 volColor = (m_SelectedIndex == 0) ? glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) : glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        Renderer::DrawString(volText, { 0.0f - (volumeWidth / 2), 0.0f }, 0.0015f, volColor, m_Font);

        float backWidth = Renderer::GetTextWidth("BACK", 0.0015f, m_Font);
        glm::vec4 backColor = (m_SelectedIndex == 1) ? glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) : glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        Renderer::DrawString("BACK", { 0.0f - (backWidth / 2), -0.3f }, 0.0015f, backColor, m_Font);

        Renderer::EndScene();
    }
};
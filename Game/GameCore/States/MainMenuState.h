#pragma once
#include "IGameState.h"

#include "Renderer/OrthographicCamera.h"
#include "Renderer/Renderer.h"
#include "Assets/ResourceManager.h"

#include "../Controllers/PlayerController.h"

class MainMenuState : public IGameState {
private:
    OrthographicCamera m_Camera;

    std::shared_ptr<Font> m_TextFont;

    float m_TextScale = 0.001f;
    float m_Time = 0.0f;

    int m_SelectedIndex = 1;
    bool m_CanContinue = false;

    std::vector<std::string> m_Options = { "CONTINUE", "NEW GAME", "OPTIONS", "QUIT" };
    std::vector<float> m_YPositions = { 0.4f, 0.1f, -0.2f, -0.5f };

    glm::vec4 m_DefaultColor = { 0.8f, 0.8f, 0.8f, 1.0f }; 
    glm::vec4 m_SelectedColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    glm::vec4 m_DisabledColor = { 0.3f, 0.3f, 0.3f, 1.0f };

public:
    MainMenuState() : m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) {}

    void OnEnter() override {
        m_TextFont = ResourceManager::Get<Font>("arial.ttf");

        // TODO: Més endavant aquí mirarem si GameSession té una partida guardada o pausada.
        m_CanContinue = false;

        m_SelectedIndex = m_CanContinue ? 0 : 1;
    }

    void OnUpdate(Timestep ts) override {
        m_Time += ts.GetSeconds();

        // Navigate Down (DOWN)
        if (PlayerController::ConsumeIfPressed(PlayerAction::Down)) {
            m_SelectedIndex++;
            if (m_SelectedIndex >= m_Options.size()) {
                m_SelectedIndex = m_CanContinue ? 0 : 1;
            }
        }

        // Navigate Up (UP)
        if (PlayerController::ConsumeIfPressed(PlayerAction::Up)) {
            m_SelectedIndex--;
            int minIndex = m_CanContinue ? 0 : 1;
            if (m_SelectedIndex < minIndex) {
                m_SelectedIndex = m_Options.size() - 1;
            }
        }

        // Accept (ENTER / SPACE)
        if (PlayerController::ConsumeIfPressed(PlayerAction::Accept) || 
            PlayerController::ConsumeIfPressed(PlayerAction::Fire)) {
            ExecuteAction();
        }
    }

    void ExecuteAction() {
        if (m_SelectedIndex == 0 && m_CanContinue) {
            // TODO: Anar a PlayingState sense reiniciar la sessió
        }
        else if (m_SelectedIndex == 1) { // NEW GAME
            if (m_RequestStateChange) m_RequestStateChange(GameStateType::Playing);
        }
        else if (m_SelectedIndex == 2) { // TODO: Options
            
        }
        else if (m_SelectedIndex == 3) { // QUIT
            Application::Get().Close();
        }
    }

    void OnRender() override {
        Renderer::SetClearColor({ 0.05f, 0.05f, 0.1f, 1.0f });
        Renderer::Clear();
        Renderer::BeginScene(m_Camera.GetViewProjectionMatrix());

        for (int i = 0; i < m_Options.size(); i++) {

            glm::vec4 color = m_DefaultColor;
            float currentScale = m_TextScale;

            if (i == 0 && !m_CanContinue) {
                color = m_DisabledColor;
            }
            else if (i == m_SelectedIndex) {
                color = m_SelectedColor;
                float scaleFluctuation = std::sin(m_Time * 4.5f) * 0.10f;
                currentScale = m_TextScale * (1.0f + scaleFluctuation);
            }

            float width = Renderer::GetTextWidth(m_Options[i], currentScale, m_TextFont);
            Renderer::DrawString(m_Options[i], { 0.0f - (width / 2), m_YPositions[i]}, currentScale, color, m_TextFont);
        }

        Renderer::EndScene();
    }
};
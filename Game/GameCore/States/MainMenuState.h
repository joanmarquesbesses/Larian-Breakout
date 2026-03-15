#pragma once
#include "IGameState.h"

#include "Renderer/OrthographicCamera.h"
#include "Renderer/Renderer.h"
#include "Assets/ResourceManager.h"

#include "../UI/TextMenu.h"
#include "../Services/SerializeScore.h"

class MainMenuState : public IGameState {
private:
    OrthographicCamera m_Camera;

    std::unique_ptr<TextMenu> m_MainMenu;
    std::shared_ptr<Font> m_Font = ResourceManager::Get<Font>("BitFont.ttf");

    GameSession* m_Session;

    float m_Time = 0.0f;
    bool m_CanContinue = false;

public:
    MainMenuState(GameSession* session) : m_Session(session), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) {}

    void OnEnter() override {
        m_Session->GetPlayer().SetHighScore(SerializeScore::LoadHighScore());

        m_CanContinue = m_Session->IsGameActive();

        std::vector<std::string> options = { "CONTINUE", "NEW GAME", "OPTIONS", "QUIT" };
        std::vector<float> yPositions = { 0.4f, 0.1f, -0.2f, -0.5f }; //options y pos

        m_MainMenu = std::make_unique<TextMenu>(options, yPositions, m_Font);
        m_MainMenu->SetSelectedIndex(m_CanContinue ? 0 : 1);
    }

    void OnUpdate(Timestep ts) override {
        m_Time += ts.GetSeconds();

        auto result = m_MainMenu->OnUpdate(m_Camera, ts, !m_CanContinue);

        // If true, user have execute a menu option
        if (result.has_value()) {
            int selectedIndex = result.value();
            ExecuteAction(selectedIndex);
        }
    }

    void ExecuteAction(int index) {
        if (index == 0 && m_CanContinue) { // CONTINUE
            if (m_RequestStateChange) m_RequestStateChange(GameStateType::Playing);
        }
        else if (index == 1) { // NEW GAME
            m_Session->SetGameActive(false);
            if (m_RequestStateChange) m_RequestStateChange(GameStateType::Playing);
        }
        else if (index == 2) { // OPTIONS
            // TODO: Obrir un submenú o estat d'opcions
        }
        else if (index == 3) { // QUIT
            Application::Get().Close();
        }
    }

    void OnRender() override {
        Renderer::SetClearColor({ 0.05f, 0.05f, 0.1f, 1.0f });
        Renderer::Clear();
        Renderer::BeginScene(m_Camera.GetViewProjectionMatrix());

        std::string scoreText = "HIGH SCORE: " + std::to_string(m_Session->GetPlayer().GetHighScore());
        Renderer::DrawString(scoreText, { -1.55f, 0.8f }, 0.001f, { 1.0f, 1.0f, 1.0f, 1.0f }, m_Font);

        // Menu render  
        if (m_MainMenu) {
            m_MainMenu->OnRender(!m_CanContinue);
        }

        Renderer::EndScene();
    }
};
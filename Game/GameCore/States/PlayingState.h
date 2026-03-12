#pragma once

#include "EngineCore/Timestep.h"
#include "Assets/ResourceManager.h"
#include "Renderer/Renderer.h"

#include "IGameState.h"
#include "GameCore/GameSession.h"
#include "GameCore/Systems/PaddleSystem.h"
#include "GameCore/Systems/PhysicsSystem.h"
#include "GameCore/Services/LevelSetup.h"

class PlayingState : public IGameState {
private:
    GameSession* m_Session;
    PaddleSystem* m_PaddleSystem;
    PhysicsSystem* m_PhysicsSystem;

    bool m_IsPaused = false;

    int m_PauseSelectedIndex = 0;
    std::shared_ptr<Font> m_Font;
    std::vector<std::string> m_PauseOptions = { "RESUME", "MAIN MENU" };

    float m_Time;

public:
    // Injecció de dependències des del Bootstrap!
    PlayingState(GameSession* session, PaddleSystem* paddleSys, PhysicsSystem* physicsSys)
        : m_Session(session), m_PaddleSystem(paddleSys), m_PhysicsSystem(physicsSys) {
    }

    void OnEnter() override {
        m_Font = ResourceManager::Get<Font>("arial.ttf");

        m_Session->GetCurrentLevel().SetGrid(5, 10);
        LevelSetup::SetUpLevel(m_Session->GetCurrentLevel());

        m_Session->GetPaddle().SetPosition({0.0f, -0.8f});
        m_Session->GetBalls().clear();
        m_Session->GetBalls().push_back(Ball({0.0f, -0.7f}, 0.1f));
        m_Session->SetIsBallInPlay(false);

        m_Session->GetPlayer().ResetLives();
        m_Session->GetPlayer().ResetScore();
    }

    void OnUpdate(Timestep ts) override {

        // Activate/Desactivate Pause
        if (PlayerController::ConsumeIfPressed(PlayerAction::Pause)) {
            m_IsPaused = !m_IsPaused;
            m_PauseSelectedIndex = 0; // Reiniciem la selecció al posar pausa
        }

        if (m_IsPaused) {
            m_Time += ts;

            if (PlayerController::ConsumeIfPressed(PlayerAction::Down)) {
                m_PauseSelectedIndex++;
                if (m_PauseSelectedIndex >= m_PauseOptions.size()) m_PauseSelectedIndex = 0;
            }
            if (PlayerController::ConsumeIfPressed(PlayerAction::Up)) {
                m_PauseSelectedIndex--;
                if (m_PauseSelectedIndex < 0) m_PauseSelectedIndex = m_PauseOptions.size() - 1;
            }

            if (PlayerController::ConsumeIfPressed(PlayerAction::Accept) || PlayerController::ConsumeIfPressed(PlayerAction::Fire)) {
                if (m_PauseSelectedIndex == 0) {
                    m_IsPaused = false;
                }
                else if (m_PauseSelectedIndex == 1) {
                    if (m_RequestStateChange) m_RequestStateChange(GameStateType::MainMenu);
                }
            }

            return;
        }

        // Paddle System
        m_PaddleSystem->Update(*m_Session, ts.GetSeconds());

        if (!m_Session->GetIsBallInPlay()) {
            // Stick Ball to Paddle
            if (!m_Session->GetBalls().empty()) {
                m_Session->GetBalls()[0].SetPosition({ m_Session->GetPaddle().GetPosition().x, m_Session->GetPaddle().GetPosition().y + 0.15f});
            }
            if (PlayerController::IsActionPressed(PlayerAction::Fire)) {
                m_Session->SetIsBallInPlay(true);
                if (!m_Session->GetBalls().empty()) m_Session->GetBalls()[0].Launch({ 1.0f, 1.5f });
            }
        }
        else {
            m_PhysicsSystem->Update(*m_Session, ts.GetSeconds());
            if (m_Session->GetPlayer().IsDead()) {
                if (m_RequestStateChange) {
                    m_RequestStateChange(GameStateType::GameOver);
                }
            }
        }
    }

    void OnRender() override {
        Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        Renderer::Clear();
        Renderer::BeginScene(m_Session->GetCamera().GetViewProjectionMatrix());

        Renderer::DrawQuad(m_Session->GetPaddle().GetPosition(), m_Session->GetPaddle().GetSize(), { 0.2f, 0.3f, 0.8f, 1.0f });

        for (auto& ball : m_Session->GetBalls()) {
            Renderer::DrawQuad(ball.GetPosition(), { ball.GetSize() * 2.0f, ball.GetSize() * 2.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
        }

        for (const auto& brick : m_Session->GetCurrentLevel().GetBricks()) {
            if (!brick.IsDestroyed()) {
                Renderer::DrawQuad(brick.GetPosition(), brick.GetSize(), brick.GetColor());
            }
        }

        if (m_IsPaused) {
            // Transparent Dark Background
            Renderer::DrawQuad({ 0.0f, 0.0f }, { 4.0f, 4.0f }, { 0.0f, 0.0f, 0.0f, 0.7f });

            float titleScale = 0.002f;
            float titleWidth = Renderer::GetTextWidth("PAUSED", titleScale, m_Font);
            Renderer::DrawString("PAUSED", { 0.0f - (titleWidth / 2.0f), 0.4f }, titleScale, { 1.0f, 1.0f, 1.0f, 1.0f }, m_Font);

            float optionBaseScale = 0.0012f;
            float startY = 0.0f;

            for (int i = 0; i < m_PauseOptions.size(); i++) {
                glm::vec4 color = { 0.8f, 0.8f, 0.8f, 1.0f };
                float currentScale = optionBaseScale;

                if (i == m_PauseSelectedIndex) {
                    color = { 1.0f, 1.0f, 1.0f, 1.0f }; // Selected
                    currentScale = optionBaseScale * (1.0f + std::sin(m_Time * 2.5) * 0.10f); 
                }

                float width = Renderer::GetTextWidth(m_PauseOptions[i], currentScale, m_Font);
                Renderer::DrawString(m_PauseOptions[i], { 0.0f - (width / 2.0f), startY - (i * 0.2f) }, currentScale, color, m_Font);
            }
        }

        Renderer::EndScene();
    }
};
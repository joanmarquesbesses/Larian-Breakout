#pragma once

#include <glm/gtc/matrix_transform.hpp>

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

    std::shared_ptr<Font> m_Font;
    std::unique_ptr<TextMenu> m_PauseMenu;

    float m_Time = 0.0f;

    // Game feel
    float m_TimeScale = 1.0f;      
    float m_HitPauseTimer = 0.0f;  
    float m_ShakeTimer = 0.0f;     
    float m_ShakeIntensity = 0.05f;
    float m_ZoomTimer = 0.0f;

public:
    // Injecció de dependències des del Bootstrap!
    PlayingState(GameSession* session, PaddleSystem* paddleSys, PhysicsSystem* physicsSys)
        : m_Session(session), m_PaddleSystem(paddleSys), m_PhysicsSystem(physicsSys) {
    }

    void OnEnter() override {
        m_Font = ResourceManager::Get<Font>("arial.ttf");

        m_PauseMenu = std::make_unique<TextMenu>(
            std::vector<std::string>{"RESUME", "MAIN MENU"},
            std::vector<float>{ 0.0f, -0.3f },
            m_Font
        );

        m_Session->GetPlayer().ResetLives();
        m_Session->GetPlayer().ResetScore();
        m_Session->SetCurrentLevelIndex(1);

        PrepareLevel();
    }

    void OnUpdate(Timestep ts) override {

        // Activate/Desactivate Pause
        if (PlayerController::ConsumeIfPressed(PlayerAction::Pause)) {
            m_IsPaused = !m_IsPaused;
            if (m_IsPaused) m_PauseMenu->SetSelectedIndex(0);
        }

        if (m_IsPaused) {
            auto result = m_PauseMenu->OnUpdate(m_Session->GetCamera(), ts.GetSeconds());

            if (result.has_value()) {
                if (result.value() == 0) {
                    m_IsPaused = false;
                }
                else if (result.value() == 1) {
                    if (m_RequestStateChange) m_RequestStateChange(GameStateType::MainMenu);
                }
            }
            return;
        }

        bool isDead = m_Session->GetPlayer().IsDead();
        m_TimeScale = isDead ? 0.2f : 1.0f;

        float realDt = ts.GetSeconds();
        float gameDt = realDt * m_TimeScale;

        // Check camera effects timers
        if (m_HitPauseTimer > 0.0f) {
            m_HitPauseTimer -= realDt;
            gameDt = 0.0f;
        }
        if (m_ShakeTimer > 0.0f) {
            m_ShakeTimer -= realDt;
        }
        if (m_ZoomTimer > 0.0f) {
            m_ZoomTimer -= realDt;
        }

        // Game Over
        if (isDead) {
            if (m_RequestStateChange) m_RequestStateChange(GameStateType::GameOver);
            if (!m_Session->GetBalls().empty()) m_Session->GetBalls()[0].Move(gameDt);
        }

        // Next level
        bool levelComplete = true;
        for (const auto& brick : m_Session->GetCurrentLevel().GetBricks()) {
            if (!brick.IsDestroyed()) {
                levelComplete = false;
                break;
            }
        }

        // Send function to GameLayer to be executed when fade in is finished
        if (levelComplete && !isDead) {
            if (m_RequestTransition) {
                m_RequestTransition(
                    [this]() {
                        int nextLevel = m_Session->GetCurrentLevelIndex() + 1;
                        m_Session->SetCurrentLevelIndex(nextLevel);
                        PrepareLevel(); 
                    }
                );
            }
            return;
        }

        // Input
        if (!isDead && !levelComplete) {
            m_PaddleSystem->Update(*m_Session, gameDt);

            if (!m_Session->GetIsBallInPlay()) {
                if (!m_Session->GetBalls().empty()) {
                    m_Session->GetBalls()[0].SetPosition({ m_Session->GetPaddle().GetPosition().x, m_Session->GetPaddle().GetPosition().y + 0.15f });
                }
                if (PlayerController::IsActionPressed(PlayerAction::Fire)) {
                    m_Session->SetIsBallInPlay(true);
                    if (!m_Session->GetBalls().empty()) m_Session->GetBalls()[0].Launch({ 1.0f, 1.5f });
                }
            }
        }

        int previousScore = m_Session->GetPlayer().GetScore();
        int previousLives = m_Session->GetPlayer().GetLives();

        // Ball Phyics
        if (m_Session->GetIsBallInPlay()) {
            m_PhysicsSystem->Update(*m_Session, gameDt);

            if (m_Session->GetPlayer().GetScore() > previousScore) {
                m_ZoomTimer = 0.15f;
            }

            if (m_Session->GetPlayer().GetLives() < previousLives) {
                m_HitPauseTimer = 0.1f; 
                m_ShakeTimer = 0.3f;     
                m_ShakeIntensity = 0.04f;
            }
        }
    }

    void OnRender() override {
        Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        Renderer::Clear();

        glm::mat4 viewProj = m_Session->GetCamera().GetViewProjectionMatrix();

        if (m_ZoomTimer > 0.0f) {
            float zoomAmount = 1.0f + (m_ZoomTimer * 0.1f);
            viewProj = glm::scale(glm::mat4(1.0f), { zoomAmount, zoomAmount, 1.0f }) * viewProj;
        }

        if (m_ShakeTimer > 0.0f) {
            // Rand value between -0.5 and 0.5 and multiplied by shakeintensity
            float offsetX = ((rand() % 100) / 100.0f - 0.5f) * m_ShakeIntensity;
            float offsetY = ((rand() % 100) / 100.0f - 0.5f) * m_ShakeIntensity;

            viewProj = glm::translate(glm::mat4(1.0f), { offsetX, offsetY, 0.0f }) * viewProj;
        }

        Renderer::BeginScene(viewProj);

        Renderer::DrawQuad(m_Session->GetPaddle().GetPosition(), m_Session->GetPaddle().GetSize(), { 0.2f, 0.3f, 0.8f, 1.0f });

        for (auto& ball : m_Session->GetBalls()) {
            Renderer::DrawQuad(ball.GetPosition(), { ball.GetSize() * 2.0f, ball.GetSize() * 2.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
        }

        for (const auto& brick : m_Session->GetCurrentLevel().GetBricks()) {
            if (!brick.IsDestroyed()) {
                Renderer::DrawQuad(brick.GetPosition(), brick.GetSize(), brick.GetColor());
            }
        }

        int lives = m_Session->GetPlayer().GetLives();

        for (int i = 0; i < lives; i++) {
            Renderer::DrawQuad({ -1.4f + (i * 0.15f), -0.85f }, { 0.1f, 0.03f }, { 0.2f, 0.8f, 0.2f, 1.0f });
        }

        if (m_IsPaused) {
            // Transparent Dark Background
            Renderer::DrawQuad({ 0.0f, 0.0f }, { 4.0f, 4.0f }, { 0.0f, 0.0f, 0.0f, 0.7f });

            // Title "PAUSED"
            float titleScale = 0.002f;
            float titleWidth = Renderer::GetTextWidth("PAUSED", titleScale, m_Font);
            Renderer::DrawString("PAUSED", { 0.0f - (titleWidth / 2.0f), 0.4f }, titleScale, { 1.0f, 1.0f, 1.0f, 1.0f }, m_Font);

            if (m_PauseMenu) {
                m_PauseMenu->OnRender();
            }
        }

        Renderer::EndScene();
    }

private:
    void PrepareLevel() {
        LevelSetup::LoadLevelFromData(m_Session->GetCurrentLevel(), m_Session->GetCurrentLevelIndex());

        m_Session->GetPaddle().SetPosition({ 0.0f, -0.8f });
        m_Session->GetPaddle().SetSize({ 0.4f, 0.05f });

        m_Session->GetBalls().clear();
        m_Session->GetBalls().push_back(Ball({ 0.0f, -0.7f }, 0.03f));
        m_Session->SetIsBallInPlay(false);
    }
};
#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "EngineCore/Timestep.h"
#include "Assets/ResourceManager.h"
#include "Renderer/Renderer.h"

#include "IGameState.h"
#include "GameCore/GameSession.h"
#include "GameCore/Systems/PaddleSystem.h"
#include "GameCore/Systems/PhysicsSystem.h"
#include "GameCore/Systems/ParticleSystem.h"
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

    ParticleSystem m_ParticleSystem;

public:
    // Injecció de dependències des del Bootstrap!
    PlayingState(GameSession* session, PaddleSystem* paddleSys, PhysicsSystem* physicsSys)
        : m_Session(session), m_PaddleSystem(paddleSys), m_PhysicsSystem(physicsSys) {
    }

    void OnEnter() override {
        m_Font = ResourceManager::Get<Font>("BitFont.ttf");

        m_PauseMenu = std::make_unique<TextMenu>(
            std::vector<std::string>{"RESUME", "MAIN MENU"},
            std::vector<float>{ 0.0f, -0.3f },
            m_Font
        );

        if (!m_Session->IsGameActive()) {
            m_Session->GetPlayer().ResetLives();
            m_Session->GetPlayer().ResetScore();
            m_Session->SetCurrentLevelIndex(0);
            PrepareLevel();

            m_Session->SetGameActive(true); 
        }

        m_IsPaused = false;
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
            m_Session->SetGameActive(false);
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
                        if (nextLevel >= LevelSetup::GetLevelCount()) {
                            nextLevel = 0; 
                        }
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
            
            auto& paddle = m_Session->GetPaddle();
            glm::vec2 pos = paddle.GetPosition();
            float halfWidth = paddle.GetSize().x / 2.0f;

            float minX = m_Session->GetCurrentLevel().GetLeftLimit() + halfWidth;
            float maxX = m_Session->GetCurrentLevel().GetRightLimit() - halfWidth;

            // std::clamp pos.x form minX to maxX
            pos.x = std::clamp(pos.x, minX, maxX);
            paddle.SetPosition(pos);

            if (!m_Session->GetIsBallInPlay()) {
                if (!m_Session->GetBalls().empty()) {
                    m_Session->GetBalls()[0].SetPosition({ m_Session->GetPaddle().GetPosition().x, m_Session->GetPaddle().GetPosition().y + 0.15f });
                }
                if (PlayerController::IsActionPressed(PlayerAction::Fire)) {
                    m_Session->SetIsBallInPlay(true);
                    if (!m_Session->GetBalls().empty()) m_Session->GetBalls()[0].Launch({ Random::Range(-1.0f, 1.0f), 1.5f });
                }
            }
        }

        int previousScore = m_Session->GetPlayer().GetScore();
        int previousLives = m_Session->GetPlayer().GetLives();

        // Ball Phyics
        if (m_Session->GetIsBallInPlay()) {
            // Check what physics report a act
            PhysicsReport report = m_PhysicsSystem->Update(*m_Session, gameDt);

            for (Brick* brick : report.hitBricks) {
                brick->TakeDamage();

                if (brick->IsDestroyed()) {
                    m_Session->GetPlayer().AddScore(100);
                    m_ZoomTimer = 0.15f;

                    if ((rand() % 100) < 20) {
                        PowerUpType randomType = static_cast<PowerUpType>(rand() % 3);
                        m_Session->GetPowerUps().push_back(PowerUp(brick->GetPosition(), { 0.1f, 0.1f }, randomType));
                    }

                    ParticleProps brickExplosion;
                    brickExplosion.Position = brick->GetPosition();
                    brickExplosion.Velocity = { 0.0f, 0.0f }; // Sortiran en totes direccions
                    brickExplosion.VelocityVariation = { 1.5f, 1.5f }; // Velocitat de sortida
                    brickExplosion.SizeBegin = 0.04f;
                    brickExplosion.SizeEnd = 0.0f; // Es fan petites fins desaparèixer
                    brickExplosion.SizeVariation = 0.01f;
                    brickExplosion.ColorBegin = brick->GetColor(); // Mateix color que el totxo!
                    brickExplosion.ColorEnd = { brick->GetColor().r, brick->GetColor().g, brick->GetColor().b, 0.0f }; // S'esvaeixen
                    brickExplosion.LifeTime = 0.5f;

                    // Emitim 10 partícules per totxo
                    for (int i = 0; i < 10; i++) {
                        m_ParticleSystem.Emit(brickExplosion);
                    }

                }
                else {
                    m_Session->GetPlayer().AddScore(10);
                }
            }

            for (PowerUp* powerUp : report.hitPowerUps) {
                switch (powerUp->GetType()) {
                    case PowerUpType::ExtraLife: 
                        if (m_Session->GetPlayer().GetLives() < 3) {
                            m_Session->GetPlayer().AddLife();
                        }
                        else {
                            m_Session->GetPlayer().AddScore(500);
                        }
                        break;
                    case PowerUpType::Enlarge: 
                        m_Session->GetPaddle().SetSize({ m_Session->GetPaddle().GetSize().x * 1.5f, m_Session->GetPaddle().GetSize().y });
                        break;
                    case PowerUpType::Multiball: 
                        SpawnMultiball(); 
                        break;
                }
                powerUp->Destroy(); 
            }
            
            PerformGarbageCollection();

            if (m_Session->GetBalls().empty()) {
                m_Session->GetPlayer().LoseLife();
                m_Session->SetIsBallInPlay(false);

                m_Session->GetPowerUps().clear();
                m_Session->GetPaddle().SetSize({ 0.4f, 0.05f });

                m_HitPauseTimer = 0.1f;
                m_ShakeTimer = 0.3f;
                m_ShakeIntensity = 0.04f;

                if (!m_Session->GetPlayer().IsDead()) {
                    m_Session->GetPaddle().SetPosition({ 0.0f, -0.8f });
                    m_Session->GetBalls().push_back(Ball({ 0.0f, -0.7f }, 0.03f));
                }
            }
        }

        m_ParticleSystem.OnUpdate(gameDt);
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

        for (const auto& powerUp : m_Session->GetPowerUps()) {
            if (!powerUp.IsDestroyed()) {
                Renderer::DrawQuad(powerUp.GetPosition(), powerUp.GetSize(), powerUp.GetColor());
            }
        }

        int lives = m_Session->GetPlayer().GetLives();

        for (int i = 0; i < lives; i++) {
            Renderer::DrawQuad({ -1.4f + (i * 0.15f), -0.85f }, { 0.1f, 0.03f }, { 0.2f, 0.8f, 0.2f, 1.0f });
        }

        std::string scoreText = "SCORE: " + std::to_string(m_Session->GetPlayer().GetScore());
        Renderer::DrawString(scoreText, { -1.57f, 0.82f }, 0.001f, { 1.0f, 1.0f, 1.0f, 1.0f }, m_Font);

        m_ParticleSystem.OnRender();

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

        m_Session->GetPowerUps().clear();

        m_Session->GetBalls().clear();
        m_Session->GetBalls().push_back(Ball({ 0.0f, -0.7f }, 0.03f));
        m_Session->SetIsBallInPlay(false);
    }

    void SpawnMultiball() {
        if (m_Session->GetBalls().empty()) return;

        int currentBallCount = m_Session->GetBalls().size();

        for (int i = 0; i < currentBallCount; i++) {
            Ball mainBall = m_Session->GetBalls()[i];

            Ball newBall1 = mainBall;
            Ball newBall2 = mainBall;

            newBall1.Launch({ -mainBall.GetVelocity().x, mainBall.GetVelocity().y });
            newBall2.Launch({ mainBall.GetVelocity().x * 1.2f, mainBall.GetVelocity().y });

            m_Session->GetBalls().push_back(newBall1);
            m_Session->GetBalls().push_back(newBall2);
        }
    }

    void PerformGarbageCollection() {
        std::erase_if(m_Session->GetPowerUps(), [](const PowerUp& p) { return p.IsDestroyed(); });
        std::erase_if(m_Session->GetBalls(), [](const Ball& b) { return b.IsDestroyed(); });
    }
};
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
#include "GameCore/Systems/GameRenderer.h"
#include "GameCore/Services/LevelSetup.h"

class PlayingState : public IGameState {
private:
    GameSession* m_Session;

    PaddleSystem* m_PaddleSystem;
    PhysicsSystem* m_PhysicsSystem;
    BrickSystem* m_BrickSystem;
	GameFlowSystem* m_GameFlowSystem;

    bool m_IsPaused = false;

    std::shared_ptr<Font> m_Font;
    std::unique_ptr<GameRenderer> m_GameRenderer;
    std::unique_ptr<TextMenu> m_PauseMenu;

    float m_Time = 0.0f;

    // Game feel
    float m_TimeScale = 1.0f;      
    float m_HitPauseTimer = 0.0f;  

    ParticleSystem m_ParticleSystem;

    std::shared_ptr<Texture2D> m_Spritesheet;

public:
    // Injecció de dependències des del Bootstrap!
    PlayingState(GameSession* session, PaddleSystem* paddleSys, PhysicsSystem* physicsSys, BrickSystem* brickSys, GameFlowSystem* gameFlowSys)
        : m_Session(session), m_PaddleSystem(paddleSys), m_PhysicsSystem(physicsSys), 
        m_BrickSystem(brickSys), m_GameFlowSystem(gameFlowSys) 
    {}

    void OnEnter() override {
        m_Font = ResourceManager::Get<Font>("BitFont.ttf");
        auto heartTex = ResourceManager::Get<Texture2D>("heart.png");
        m_Spritesheet = ResourceManager::Get<Texture2D>("spritesheet-breakout.png");
		auto nebulaTex = ResourceManager::Get<Texture2D>("nebula.png");

        m_GameRenderer = std::make_unique<GameRenderer>(m_Font, &m_ParticleSystem, heartTex, nebulaTex);

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
        m_TimeScale = isDead ? 0.4f : 1.0f;

        float realDt = ts.GetSeconds();
        float gameDt = realDt * m_TimeScale;

		// Check hit pause
        if (m_HitPauseTimer > 0.0f) {
            m_HitPauseTimer -= realDt;
            gameDt = 0.0f;
        }

        FlowState state = m_GameFlowSystem->Update(*m_Session);

        if (state == FlowState::GameOver) {
            if (m_RequestStateChange) m_RequestStateChange(GameStateType::GameOver);
            m_Session->SetGameActive(false);
            m_HitPauseTimer = 1.0f;
            if (!m_Session->GetBalls().empty()) m_Session->GetBalls()[0].Move(gameDt);
        }
        else if (state == FlowState::LevelComplete) {
            if (m_RequestTransition) {
                m_RequestTransition([this]() {
                    int nextLevel = m_Session->GetCurrentLevelIndex() + 1;
                    if (nextLevel >= LevelSetup::GetLevelCount()) {
                        nextLevel = 0;
                    }
                    m_Session->SetCurrentLevelIndex(nextLevel);
                    PrepareLevel();
                    });
            }
            return;
        }
        else if (state == FlowState::BallLost) {
            m_HitPauseTimer = 0.1f;
            m_GameRenderer->TriggerShake(0.3f, 0.04f);

            m_Session->SetIsBallInPlay(false);
            m_Session->GetPowerUps().clear();

            m_Session->GetPaddle().SetSize({ 0.6f, 0.07f });
            m_Session->GetPaddle().SetPosition({ 0.0f, -0.8f });

            Ball newBall({ 0.0f, -0.7f }, 0.03f);
            if (m_Spritesheet) {
                auto ballTex = SubTexture2D::CreateFromPixelCoords(m_Spritesheet, 32.0f, 32.0f, 16.0f, 16.0f);
                newBall.SetTexture(ballTex);
            }
            m_Session->GetBalls().push_back(newBall);
        }

        bool stopInput = (state != FlowState::Playing && state != FlowState::BallLost);

        // Input i Moviment (Només si la partida continua)
        if (!stopInput) {
            m_PaddleSystem->Update(*m_Session, gameDt);

            auto& paddle = m_Session->GetPaddle();
            glm::vec2 pos = paddle.GetPosition();
            float halfWidth = paddle.GetSize().x / 2.0f;

            float minX = m_Session->GetCurrentLevel().GetLeftLimit() + halfWidth;
            float maxX = m_Session->GetCurrentLevel().GetRightLimit() - halfWidth;

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

		// Physics and collisions (Just if playmode)
        if (m_Session->GetIsBallInPlay()) {
            PhysicsReport report = m_PhysicsSystem->Update(*m_Session, gameDt);

            std::vector<Brick*> bricksToProcess = report.hitBricks;

            for (size_t i = 0; i < bricksToProcess.size(); i++) {
                Brick* brick = bricksToProcess[i];

                if (brick->IsDying()) continue;

                brick->TakeDamage();

                if (brick->GetHealth() <= 0) {
                    brick->StartDying();
                    m_Session->GetPlayer().AddScore(100);
                    m_GameRenderer->TriggerZoom(0.15f);

                    if (brick->IsExplosive()) {
                        m_GameRenderer->TriggerShake(0.5f, 0.08f);
                        auto neighbors = m_BrickSystem->GetBricksInExplosion(*m_Session, brick);

                        for (Brick* neighbor : neighbors) {
                            neighbor->SetHealth(0);
                            bricksToProcess.push_back(neighbor);
                        }
                    }

                    if ((rand() % 100) < 20) {
                        PowerUpType randomType = static_cast<PowerUpType>(rand() % 3);
                        PowerUp newPowerUp(brick->GetPosition(), { 0.1f, 0.1f }, randomType);
                       
                        if (randomType == PowerUpType::ExtraLife) {
                            newPowerUp.SetTexture(ResourceManager::Get<Texture2D>("heart.png"));
                        }

                        m_Session->GetPowerUps().push_back(newPowerUp);
                    }

                    ParticleProps brickExplosion;
                    brickExplosion.Position = brick->GetPosition();
                    brickExplosion.Velocity = { 0.0f, 0.0f };
                    brickExplosion.VelocityVariation = { 1.5f, 1.5f };
                    brickExplosion.SizeBegin = 0.04f;
                    brickExplosion.SizeEnd = 0.0f;
                    brickExplosion.SizeVariation = 0.01f;
                    brickExplosion.ColorBegin = brick->GetColor();
                    brickExplosion.ColorEnd = { brick->GetColor().r, brick->GetColor().g, brick->GetColor().b, 0.0f };
                    brickExplosion.LifeTime = 0.5f;

                    for (int i = 0; i < 10; i++) {
                        m_ParticleSystem.Emit(brickExplosion);
                    }
                }
                else {
                    m_Session->GetPlayer().AddScore(10);
                    m_BrickSystem->UpdateBrickTexture(*brick, m_Spritesheet);
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

            m_BrickSystem->Update(*m_Session, gameDt);
            PerformGarbageCollection();
        }

        m_ParticleSystem.OnUpdate(gameDt);
        m_GameRenderer->UpdateEffects(realDt);
    }

    void OnRender() override {
		m_GameRenderer->Render(*m_Session, m_IsPaused, m_PauseMenu.get());
    }

private:
    void PrepareLevel() {
        LevelSetup::LoadLevelFromData(m_Session->GetCurrentLevel(), m_Session->GetCurrentLevelIndex());

        m_BrickSystem->SetupBricks(*m_Session, m_Spritesheet);
        for (auto& brick : m_Session->GetCurrentLevel().GetBricks()) {
            if (brick.GetMaxHealth() == 3) {
                brick.SetExplosive(true);
            }
        }

        auto paddleTex = SubTexture2D::CreateFromPixelCoords(m_Spritesheet, 32.0f, 112.0f, 162.0f, 14.0f);
        m_Session->GetPaddle().SetTexture(paddleTex);

        m_Session->GetPaddle().SetPosition({ 0.0f, -0.8f });
        m_Session->GetPaddle().SetSize({ 0.6f, 0.07f });

        m_Session->GetPowerUps().clear();

        m_Session->GetBalls().clear();
        m_Session->GetBalls().push_back(Ball({ 0.0f, -0.7f }, 0.03f));
		auto ballTex = SubTexture2D::CreateFromPixelCoords(m_Spritesheet, 32.0f, 32.0f, 16.0f, 16.0f);
		m_Session->GetBalls()[0].SetTexture(ballTex);
        m_Session->SetIsBallInPlay(false);
    }

    void SpawnMultiball() {
        if (m_Session->GetBalls().empty()) return;

        int currentBallCount = m_Session->GetBalls().size();

        for (int i = 0; i < currentBallCount; i++) {
            Ball mainBall = m_Session->GetBalls()[i];

            Ball newBall1 = mainBall;
            Ball newBall2 = mainBall;

			newBall1.SetTexture(mainBall.GetTexture());
			newBall2.SetTexture(mainBall.GetTexture());

            newBall1.Launch({ -mainBall.GetVelocity().x, mainBall.GetVelocity().y });
            newBall2.Launch({ mainBall.GetVelocity().x * 1.2f, mainBall.GetVelocity().y });

            m_Session->GetBalls().push_back(newBall1);
            m_Session->GetBalls().push_back(newBall2);
        }
    }

    void PerformGarbageCollection() {
        std::erase_if(m_Session->GetPowerUps(), [](const PowerUp& p) { return p.IsDestroyed(); });
        std::erase_if(m_Session->GetBalls(), [](const Ball& b) { return b.IsDestroyed(); });

        auto& bricks = m_Session->GetCurrentLevel().GetBricks();
        std::erase_if(bricks, [](const Brick& b) { return b.IsDestroyed(); });
    }

};
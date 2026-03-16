#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "EngineCore/Timestep.h"
#include "Assets/ResourceManager.h"
#include "Renderer/Renderer.h"
#include "Utils/Random.h"
#include "Audio/AudioEngine.h"

#include "IGameState.h"
#include "GameCore/GameSession.h"
#include "GameCore/Systems/PaddleSystem.h"
#include "GameCore/Systems/PhysicsSystem.h"
#include "GameCore/Systems/ParticleSystem.h"
#include "GameCore/Systems/GameRenderer.h"
#include "GameCore/Services/LevelSetup.h"
#include "GameCore/Resources/ParticlePresets.h"

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

    std::shared_ptr<AudioClip> m_BallBounce;
    std::shared_ptr<AudioClip> m_BrickDestroyed;
    std::shared_ptr<AudioClip> m_LoseLife;
    std::shared_ptr<AudioClip> m_LostGame;
    std::shared_ptr<AudioClip> m_NextLevel;
    std::shared_ptr<AudioClip> m_PowerUp;

    float m_Time = 0.0f;

    // Game feel
    float m_TimeScale = 1.0f;      
    float m_TransitionTimer = 0.0f;
    FlowState m_PendingTransitionState = FlowState::Playing;

    float m_AimAngle = 0.0f;
    float m_AimDir = 1.0f;

    ParticleSystem m_ParticleSystem;
    ParticleSystem m_BgParticleSystem;

    std::shared_ptr<Texture2D> m_Spritesheet;
    std::shared_ptr<SubTexture2D> m_BallTexture;
    std::shared_ptr<SubTexture2D> m_PaddleTexture;

public:
    // Injecció de dependències des del Bootstrap!
    PlayingState(GameSession* session, PaddleSystem* paddleSys, PhysicsSystem* physicsSys, BrickSystem* brickSys, GameFlowSystem* gameFlowSys)
        : m_Session(session), m_PaddleSystem(paddleSys), m_PhysicsSystem(physicsSys), 
        m_BrickSystem(brickSys), m_GameFlowSystem(gameFlowSys) 
    {}

    void OnEnter() override {
        m_Font = ResourceManager::Get<Font>("Assets/Font/BitFont.ttf");
        m_Spritesheet = ResourceManager::Get<Texture2D>("Assets/Textures/spritesheet-breakout.png");
        auto heartTex = ResourceManager::Get<Texture2D>("Assets/Textures/heart.png");
		auto nebulaTex = ResourceManager::Get<Texture2D>("Assets/Textures/nebula.png");
		auto bombTex = ResourceManager::Get<Texture2D>("Assets/Textures/bomb.png");

		m_BallBounce = ResourceManager::Get<AudioClip>("Assets/SFX/BallBounce.mp3");
		m_BrickDestroyed = ResourceManager::Get<AudioClip>("Assets/SFX/BrickDestroyed.mp3");
		m_LoseLife = ResourceManager::Get<AudioClip>("Assets/SFX/LoseLife.mp3");
		m_LostGame = ResourceManager::Get<AudioClip>("Assets/SFX/LostGame.mp3");
		m_NextLevel = ResourceManager::Get<AudioClip>("Assets/SFX/NextLevel.mp3");
		m_PowerUp = ResourceManager::Get<AudioClip>("Assets/SFX/PowerUp.mp3");

        m_PaddleTexture = SubTexture2D::CreateFromPixelCoords(m_Spritesheet, 32.0f, 112.0f, 162.0f, 14.0f);
        m_BallTexture = SubTexture2D::CreateFromPixelCoords(m_Spritesheet, 32.0f, 32.0f, 16.0f, 16.0f);

        m_GameRenderer = std::make_unique<GameRenderer>(m_Font, &m_ParticleSystem, &m_BgParticleSystem, heartTex, nebulaTex, bombTex);

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
        else {
            m_IsPaused = true;
            m_PauseMenu->SetSelectedIndex(0);
        }

        for (int i = 0; i < 40; i++) {
            ParticleProps initialStar = ParticlePresets::GetStar();
            float randomY = ((rand() % 200) / 100.0f) - 1.0f;
            initialStar.Position.y = randomY;
            initialStar.LifeTime = 10.0f + ((rand() % 1500) / 100.0f);
            m_BgParticleSystem.Emit(initialStar);
        }
    }

    void OnUpdate(Timestep ts) override {

        // Activate/Desactivate Pause
        if (PlayerController::ConsumeIfPressed(PlayerAction::Pause)) {
            m_IsPaused = !m_IsPaused;
            if (m_IsPaused) m_PauseMenu->SetSelectedIndex(0);
        }

		// Pause menu update and input (Just if paused)
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

        if (PlayerController::ConsumeIfPressed(PlayerAction::SkipLevel) && m_PendingTransitionState == FlowState::Playing) {
            m_PendingTransitionState = FlowState::LevelComplete;
            if (m_NextLevel) AudioEngine::Play(m_NextLevel->GetPath());
        }

        float realDt = ts.GetSeconds();
        float gameDt = realDt * m_TimeScale;

        FlowState state = m_GameFlowSystem->Update(*m_Session);

		// Update game flow and check for state changes (Level complete, ball lost, game over)
        if ((state == FlowState::GameOver || state == FlowState::LevelComplete) && m_PendingTransitionState == FlowState::Playing) {
            m_PendingTransitionState = state;
            m_TimeScale = 0.2f;       

            if (state == FlowState::GameOver) {
                m_TransitionTimer = 2.0f;
                if (m_LostGame) AudioEngine::Play(m_LostGame->GetPath());
            }

        }
        else if (state == FlowState::BallLost && m_PendingTransitionState == FlowState::Playing) {
            m_GameRenderer->TriggerShake(0.3f, 0.04f);

            if (m_LoseLife) AudioEngine::Play(m_LoseLife->GetPath());

            m_Session->SetIsBallInPlay(false);
            m_Session->GetPowerUps().clear();

            m_Session->GetPaddle().SetSize({ 0.6f, 0.07f });
            m_Session->GetPaddle().SetPosition({ 0.0f, -0.8f });

            Ball newBall({ 0.0f, -0.7f }, 0.03f);
            if (m_Spritesheet) {
                newBall.SetTexture(m_BallTexture);
            }
            m_Session->GetBalls().push_back(newBall);
        }

        bool stopInput = (state == FlowState::BallLost || m_PendingTransitionState == FlowState::GameOver);

		// Player input and paddle movement (Just if playmode or ball lost, not on level complete or game over)
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
                    m_AimAngle += 1.2f * realDt * m_AimDir;
                    if (m_AimAngle > 1.0f) { m_AimAngle = 1.0f; m_AimDir = -1.0f; } // 1.0 radian = aprox 57 graus
                    if (m_AimAngle < -1.0f) { m_AimAngle = -1.0f; m_AimDir = 1.0f; }
                }
                if (PlayerController::IsActionPressed(PlayerAction::Fire)) {
                    m_Session->SetIsBallInPlay(true);
                    if (!m_Session->GetBalls().empty()) {
                        glm::vec2 launchDir = { std::sin(m_AimAngle), std::cos(m_AimAngle) };
                        m_Session->GetBalls()[0].Launch(launchDir * 1.0f);
                    }
                }
            }
        }

		// Physics and collisions (Just if playmode)
        if (m_Session->GetIsBallInPlay()) {
            PhysicsReport report = m_PhysicsSystem->Update(*m_Session, gameDt);

            std::vector<Brick*> bricksToProcess = report.hitBricks;

            if (report.ballBounced || !report.hitBricks.empty()) {
                if (m_BallBounce) AudioEngine::Play(m_BallBounce->GetPath());
            }

            bool playBrickDestroyedSFX = false;

            for (size_t i = 0; i < bricksToProcess.size(); i++) {
                Brick* brick = bricksToProcess[i];

                if (brick->IsDying()) continue;

				if (brick->GetMaxHealth() == -1) { // Indestructible brick, just skip it
                    continue; 
                }

                brick->TakeDamage();

                for (auto& ball : m_Session->GetBalls()) {
                    glm::vec2 currentVel = ball.GetVelocity();
                    float currentSpeed = glm::length(currentVel);
                    if (currentSpeed < 2.2f) {
                        ball.SetVelocity(currentVel * 1.015f);
                    }
                }

                if (brick->GetHealth() <= 0) {
                    brick->StartDying();
                    m_Session->GetPlayer().AddScore(100);
                    m_GameRenderer->TriggerZoom(0.15f);
                    m_GameRenderer->AddFloatingText("100", brick->GetPosition(), { 1.0f, 0.8f, 0.2f, 1.0f });

                    bool isLastBrick = true;
                    for (auto& otherBrick : m_Session->GetCurrentLevel().GetBricks()) {
                        if (otherBrick.GetHealth() > 0 && otherBrick.GetMaxHealth() != -1 && &otherBrick != brick) {
                            isLastBrick = false;
                            break;
                        }
                    }

					// If this was the last brick, start the level complete transition and play the next level sfx
                    if (isLastBrick && m_PendingTransitionState == FlowState::Playing) {
                        m_TimeScale = 0.2f; 
                        if (m_NextLevel) AudioEngine::Play(m_NextLevel->GetPath());
                    }

                    if (brick->IsExplosive()) {
                        m_GameRenderer->TriggerShake(0.3f, 0.08f);
                        auto neighbors = m_BrickSystem->GetBricksInExplosion(*m_Session, brick);

                        for (Brick* neighbor : neighbors) {
                            neighbor->SetHealth(0);
                            bricksToProcess.push_back(neighbor);
                        }
                        playBrickDestroyedSFX = true;
                    }
                    else {
                        playBrickDestroyedSFX = true;
                    }

                    if (Random::Range(1,100) < 33) {
                        PowerUpType randomType = static_cast<PowerUpType>(rand() % 3);
                        PowerUp newPowerUp(brick->GetPosition(), { 0.1f, 0.1f }, randomType);

                        switch (randomType)
                        {
                        case PowerUpType::ExtraLife:
                            newPowerUp.SetTexture(ResourceManager::Get<Texture2D>("Assets/Textures/heart.png"));
                            break;
                        case PowerUpType::Enlarge:
							newPowerUp.SetSubTexture(m_PaddleTexture);
                            newPowerUp.SetSize({ 0.2f, 0.05f });
                            break;
                        case PowerUpType::Multiball:
							newPowerUp.SetSubTexture(m_BallTexture);
                            break;
                        default:
                            break;
                        }

                        m_Session->GetPowerUps().push_back(newPowerUp);
                    }

                    ParticleProps explosion = ParticlePresets::GetBrickExplosion(brick->GetPosition(), brick->GetColor());
                    for (int i = 0; i < 10; i++) {
                        m_ParticleSystem.Emit(explosion);
                    }
                }
                else {
                    m_Session->GetPlayer().AddScore(10);
                    m_BrickSystem->UpdateBrickTexture(*brick, m_Spritesheet);
                    m_GameRenderer->AddFloatingText("10", brick->GetPosition(), { 1.0f, 1.0f, 1.0f, 1.0f });
                }
            }

            if (playBrickDestroyedSFX && m_BrickDestroyed) {
                AudioEngine::Play(m_BrickDestroyed->GetPath());
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
                case PowerUpType::Enlarge: {
                    float currentWidth = m_Session->GetPaddle().GetSize().x;
                    float newWidth = std::min(currentWidth * 1.5f, 1.0f);
                    m_Session->GetPaddle().SetSize({ newWidth, m_Session->GetPaddle().GetSize().y });
                    break;
                }
                case PowerUpType::Multiball:
                    SpawnMultiball();
                    break;
                }
                powerUp->Destroy();
				if(m_PowerUp) AudioEngine::Play(m_PowerUp->GetPath());
            }

            for (const auto& powerUp : m_Session->GetPowerUps()) {
                if (!powerUp.IsDestroyed()) {
                    m_ParticleSystem.Emit(ParticlePresets::GetPowerUpTrail(powerUp.GetPosition(), powerUp.GetColor()));
                }
            }

            m_BrickSystem->Update(*m_Session, gameDt);
            PerformGarbageCollection();
        }

		// Handle level complete or game over transitions
        if (m_PendingTransitionState != FlowState::Playing) {

            if (m_TransitionTimer > 0.0f) {
                m_TransitionTimer -= realDt;
            }

            if (m_TransitionTimer <= 0.0f || m_PendingTransitionState == FlowState::LevelComplete) {

                if (m_PendingTransitionState == FlowState::GameOver) {
                    if (m_RequestStateChange) m_RequestStateChange(GameStateType::GameOver);
                    m_Session->SetGameActive(false);
                }
                else if (m_PendingTransitionState == FlowState::LevelComplete) {
                    if (m_RequestTransition) {
                        m_RequestTransition([this]() {
                            int nextLevel = m_Session->GetCurrentLevelIndex() + 1;
                            if (nextLevel >= LevelSetup::GetLevelCount()) {
                                nextLevel = 0;
                            }
                            m_Session->SetCurrentLevelIndex(nextLevel);
                            PrepareLevel();
                            m_TimeScale = 1.0f;
                            m_PendingTransitionState = FlowState::Playing;
                            });
                    }
                }
            }
        }
		
        if (Random::Range(0.0f, 100.f) < 1) {
            m_BgParticleSystem.Emit(ParticlePresets::GetStar());
        }

        m_ParticleSystem.OnUpdate(gameDt);
        m_BgParticleSystem.OnUpdate(gameDt);

        m_GameRenderer->UpdateEffects(realDt, gameDt);
    }

    void OnRender() override {
        float currentAim = m_Session->GetIsBallInPlay() ? -999.0f : m_AimAngle;
		m_GameRenderer->Render(*m_Session, m_IsPaused, m_PauseMenu.get(), currentAim);
    }

private:
	// Load level data, setup bricks, paddle and ball for the current level
    void PrepareLevel() {
        LevelSetup::LoadLevelFromData(m_Session->GetCurrentLevel(), m_Session->GetCurrentLevelIndex());

        m_BrickSystem->SetupBricks(*m_Session, m_Spritesheet);
        for (auto& brick : m_Session->GetCurrentLevel().GetBricks()) {
			if (brick.GetMaxHealth() == -1) { // Indestructible brick
                auto invincibleTex = SubTexture2D::CreateFromPixelCoords(m_Spritesheet, 32.0f, 288.0f, 32.0f, 16.0f);
                brick.SetIdleTexture(invincibleTex);
            }
        }

        m_Session->GetPaddle().SetTexture(m_PaddleTexture);

        m_Session->GetPaddle().SetPosition({ 0.0f, -0.8f });
        m_Session->GetPaddle().SetSize({ 0.6f, 0.07f });

        m_Session->GetPowerUps().clear();

        m_Session->GetBalls().clear();
        m_Session->GetBalls().push_back(Ball({ 0.0f, -0.7f }, 0.03f));
		m_Session->GetBalls()[0].SetTexture(m_BallTexture);
        m_Session->SetIsBallInPlay(false);
    }

	// Multiball power-up: Spawn 2 new balls for each existing ball, with slightly different velocities
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

	// Remove destroyed entities
    void PerformGarbageCollection() {
        std::erase_if(m_Session->GetPowerUps(), [](const PowerUp& p) { return p.IsDestroyed(); });

        for (const auto& b : m_Session->GetBalls()) {
            if (b.IsDestroyed()) {
                ParticleProps deathEffect = ParticlePresets::GetBallDeath(b.GetPosition());
                for (int i = 0; i < 20; i++) {
                    m_ParticleSystem.Emit(deathEffect);
                }
            }
        }

        std::erase_if(m_Session->GetBalls(), [](const Ball& b) { return b.IsDestroyed(); });

        auto& bricks = m_Session->GetCurrentLevel().GetBricks();
        std::erase_if(bricks, [](const Brick& b) { return b.IsDestroyed(); });
    }

};
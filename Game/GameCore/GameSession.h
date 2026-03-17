#pragma once

#include <vector>

#include "Renderer/OrthographicCamera.h"
#include "Entities/Player.h"
#include "Entities/Paddle.h"
#include "Entities/Ball.h"
#include "Entities/PowerUp.h"
#include "Entities/Level.h"

enum class GameState {
    MainMenu,
    Playing,
    Paused,
    GameOver,
    Victory
};

// Acts as the single source of truth for the current state of the game world.
// Contains all pure data entities and the camera.
class GameSession {
private:
    OrthographicCamera m_Camera;

    Player m_Player;
    Paddle m_Paddle;
    std::vector<Ball> m_Balls;
    std::vector<PowerUp> m_PowerUps;

    Level m_CurrentLevel;
    int m_CurrentLevelIndex = 0;

    bool m_IsBallInPlay = false;
    bool m_IsGameActive = false; // True if a run is currently ongoing (allows "Continue" from Main Menu)

public:
    GameSession()
        : m_Camera(-1.6f, 1.6f, -0.9f, 0.9f),
        m_CurrentLevel(-1.6f, 1.6f, 0.9f, -0.9f)
    {}

    const OrthographicCamera& GetCamera() const { return m_Camera; }

    Player& GetPlayer() { return m_Player; }
    Paddle& GetPaddle() { return m_Paddle; }
    std::vector<Ball>& GetBalls() { return m_Balls; }
    std::vector<PowerUp>& GetPowerUps() { return m_PowerUps; }

    Level& GetCurrentLevel() { return m_CurrentLevel; }
    int GetCurrentLevelIndex() const { return m_CurrentLevelIndex; }
    void SetCurrentLevelIndex(int index) { m_CurrentLevelIndex = index; }

    void SetIsBallInPlay(bool isBallInPlay) { m_IsBallInPlay = isBallInPlay; }
    bool GetIsBallInPlay() { return m_IsBallInPlay; }

    bool IsGameActive() const { return m_IsGameActive; }
    void SetGameActive(bool active) { m_IsGameActive = active; }
};
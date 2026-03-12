#pragma once

#include <vector>

#include "Renderer/OrthographicCamera.h"
#include "Entities/Player.h"
#include "Entities/Paddle.h"
#include "Entities/Ball.h"
#include "Entities/Level.h"

enum class GameState {
    MainMenu,
    Playing,
    Paused,
    GameOver,
    Victory
};

class GameSession {
private:
    OrthographicCamera m_Camera;
    Player m_Player;
    Paddle m_Paddle;
    Level m_CurrentLevel;
    std::vector<Ball> m_Balls;
    bool m_IsBallInPlay = false;

public:
    GameSession()
        : m_Camera(-1.6f, 1.6f, -0.9f, 0.9f),
        m_CurrentLevel(-1.6f, 1.6f, 0.9f, -0.9f)
    {}

    const OrthographicCamera& GetCamera() const { return m_Camera; }
    Player& GetPlayer() { return m_Player; }
    Paddle& GetPaddle() { return m_Paddle; }
    std::vector<Ball>& GetBalls() { return m_Balls; }
    Level& GetCurrentLevel() { return m_CurrentLevel; }

    void SetIsBallInPlay(bool isBallInPlay) { m_IsBallInPlay = isBallInPlay; }
    void ToggleIsBallInPlay() { m_IsBallInPlay != m_IsBallInPlay; }
    bool GetIsBallInPlay() { return m_IsBallInPlay; }
};
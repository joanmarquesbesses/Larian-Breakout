#pragma once

#include "Timestep.h"
#include "Input/Input.h"
#include "Renderer/OrthographicCamera.h"

#include "Entities/Player.h"
#include "Entities/Paddle.h"
#include "Entities/Ball.h"
#include "Entities/Level.h"

#include "LevelFactory.h"
#include "PhysicsService.h"

enum class GameState {
    MainMenu,
    Playing,
    Paused,
    GameOver,
    Victory
};

class GameSession {
private:
    GameState m_CurrentState = GameState::Playing;

    OrthographicCamera m_Camera;

    Player m_Player;
    Paddle m_Paddle;
    Ball m_Ball;
    Level m_Level;

public:
    GameSession()
        : m_Camera(-1.6f, 1.6f, -0.9f, 0.9f),
        m_Level(-1.6f, 1.6f, 0.9f, -0.9f)
    {}

    void Init() {
        m_CurrentState = GameState::Playing;

        m_Paddle = Paddle({ 0.0f, -0.8f }, { 0.6f, 0.1f }, 2.0f);
        m_Ball = Ball({ 0.0f, -0.7f }, 0.05f);
        m_Ball.Launch({ 1.0f, 1.5f });

        LevelFactory::GenerateBasicLevel(m_Level);
    }

    void Update(Timestep ts) {
        if (m_CurrentState != GameState::Playing) return;

        if (Input::IsKeyPressed(KeyCode::A) || Input::IsKeyPressed(KeyCode::Left)) {
            m_Paddle.MoveLeft(ts.GetSeconds());
        }
        else if (Input::IsKeyPressed(KeyCode::D) || Input::IsKeyPressed(KeyCode::Right)) {
            m_Paddle.MoveRight(ts.GetSeconds());
        }

        PhysicsService::Update(m_Paddle, m_Ball, m_Level, m_Player, ts.GetSeconds());
    }

    const OrthographicCamera& GetCamera() const { return m_Camera; }
    const Paddle& GetPaddle() const { return m_Paddle; }
    const Ball& GetBall() const { return m_Ball; }
    const Level& GetLevel() const { return m_Level; }

private:
    bool CheckAABB(glm::vec2 posA, glm::vec2 sizeA, glm::vec2 posB, glm::vec2 sizeB) {
        bool collisionX = posA.x + sizeA.x / 2.0f >= posB.x - sizeB.x / 2.0f && posB.x + sizeB.x / 2.0f >= posA.x - sizeA.x / 2.0f;
        bool collisionY = posA.y + sizeA.y / 2.0f >= posB.y - sizeB.y / 2.0f && posB.y + sizeB.y / 2.0f >= posA.y - sizeA.y / 2.0f;
        return collisionX && collisionY;
    }
};
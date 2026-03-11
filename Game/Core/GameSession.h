#pragma once

#include "Timestep.h"
#include "Renderer/OrthographicCamera.h"

#include "Entities/Player.h"
#include "Entities/Paddle.h"
#include "Entities/Ball.h"
#include "Entities/Level.h"

#include "LevelFactory.h"
#include "PhysicsService.h"
#include "PlayerController.h"

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

    bool m_BallInPlay = false;

public:
    GameSession()
        : m_Camera(-1.6f, 1.6f, -0.9f, 0.9f),
        m_Level(-1.6f, 1.6f, 0.9f, -0.9f)
    {}

    void Init() {
        m_CurrentState = GameState::Playing;
        m_Player = Player();    
        m_Paddle = Paddle({ 0.0f, -0.8f }, { 0.6f, 0.1f }, 2.0f);
        m_Ball = Ball({ 0.0f, -0.7f }, 0.05f);

        m_Level.SetGrid(5, 10);
        LevelFactory::SetUpLevel(m_Level);

        ResetBall();
    }

    void Update(Timestep ts) {
        if (m_CurrentState != GameState::Playing) return;

        if (!m_BallInPlay) {
            // Ball sticked to Paddle
            m_Ball.SetPosition({
                m_Paddle.GetPosition().x,
                m_Paddle.GetPosition().y + m_Paddle.GetSize().y / 2.0f + m_Ball.GetSize()
                });
        }
        else {
            // Ball physics
            PhysicsService::Update(m_Paddle, m_Ball, m_Level, m_Player, ts.GetSeconds()); //physiscs loop/system
            // mirar: cridar physicssystem que sapiga gestionar les fisiques de les diferentes enitats fisiques
            // ha de dir a tothom que tingui fisiques calculeuvos
            // 
            // un servei no te update, el crides quan el necesites.

            // Lives
            if (m_Ball.GetPosition().y < m_Level.GetBottomLimit()) {
                m_Player.LoseLife();

                if (m_Player.IsDead()) {
                    m_CurrentState = GameState::GameOver;
                    std::cout << "GAME OVER! Puntuació final: " << m_Player.Score << "\n";
                }
                else {
                    ResetBall();
                }
            }
        }
    }

    void ResetBall() {
        m_BallInPlay = false;
        m_Paddle.SetPosition({ 0.0f, -0.8f });
        m_Ball.Launch({ 0.0f, 0.0f });
    }

    void FireBall() {
        if (!m_BallInPlay && m_CurrentState == GameState::Playing) {
            m_BallInPlay = true;
            m_Ball.Launch({ 1.0f, 1.5f });
        }
    }

    const OrthographicCamera& GetCamera() const { return m_Camera; }
    const Paddle& GetPaddle() const { return m_Paddle; }
    Paddle& GetPaddleMutable() { return m_Paddle; }
    const Ball& GetBall() const { return m_Ball; }
    const Level& GetLevel() const { return m_Level; }

    GameState GetGameState() const { return m_CurrentState; }

private:
    bool CheckAABB(glm::vec2 posA, glm::vec2 sizeA, glm::vec2 posB, glm::vec2 sizeB) {
        bool collisionX = posA.x + sizeA.x / 2.0f >= posB.x - sizeB.x / 2.0f && posB.x + sizeB.x / 2.0f >= posA.x - sizeA.x / 2.0f;
        bool collisionY = posA.y + sizeA.y / 2.0f >= posB.y - sizeB.y / 2.0f && posB.y + sizeB.y / 2.0f >= posA.y - sizeA.y / 2.0f;
        return collisionX && collisionY;
    }
};
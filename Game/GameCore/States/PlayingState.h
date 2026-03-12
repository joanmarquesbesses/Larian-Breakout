#pragma once

#include "EngineCore/Timestep.h"
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

public:
    // Injecció de dependències des del Bootstrap!
    PlayingState(GameSession* session, PaddleSystem* paddleSys, PhysicsSystem* physicsSys)
        : m_Session(session), m_PaddleSystem(paddleSys), m_PhysicsSystem(physicsSys) {
    }

    void OnEnter() override {

        m_Session->GetCurrentLevel().SetGrid(5, 10);
        LevelSetup::SetUpLevel(m_Session->GetCurrentLevel());

        m_Session->GetPaddle().SetPosition({0.0f, -0.8f});
        m_Session->GetBalls().clear();
        m_Session->GetBalls().push_back(Ball({0.0f, -0.7f}, 0.1f));
        m_Session->SetIsBallInPlay(false);
    }

    void OnUpdate(Timestep ts) override {
        // 1. Executem els Sistemes
        m_PaddleSystem->Update(*m_Session, ts.GetSeconds());

        if (!m_Session->GetIsBallInPlay()) {
            // Bola enganxada a la pala
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

            // Si hem perdut, hauríem de canviar a GameOverState (ho farem més endavant)
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

        Renderer::EndScene();
    }
};
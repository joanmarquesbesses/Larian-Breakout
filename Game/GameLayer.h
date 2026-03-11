#include "Layer.h"
#include "Renderer/Renderer.h"
#include "Core/GameSession.h"

class GameLayer : public Layer
{
private:
    GameSession m_Session;

public:

    GameLayer() : Layer("GameLayer") {}

    void OnAttach() override
    {
        m_Session.Init();
    }

    void OnUpdate(Timestep ts) override
    {
        m_Session.Update(ts);

        Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        Renderer::Clear();

        Renderer::BeginScene(m_Session.GetCamera().GetViewProjectionMatrix());

        auto& paddle = m_Session.GetPaddle();
        Renderer::DrawQuad(paddle.GetPosition(), paddle.GetSize(), { 0.2f, 0.3f, 0.8f, 1.0f });

        auto& ball = m_Session.GetBall();
        Renderer::DrawQuad(ball.GetPosition(), { ball.GetSize(), ball.GetSize() }, { 1.0f, 1.0f, 1.0f, 1.0f });

        for (const auto& brick : m_Session.GetLevel().GetBricks()) {
            if (!brick.IsDestroyed()) {
                Renderer::DrawQuad(brick.GetPosition(), brick.GetSize(), { 0.8f, 0.2f, 0.2f, 1.0f });
            }
        }

        Renderer::EndScene();
    }
};
#include "EngineCore/Layer.h"
#include "GameCore/GameSession.h"
#include "GameCore/Systems/PaddleSystem.h"
#include "GameCore/Systems/PhysicsSystem.h"
#include "GameCore/States/PlayingState.h"

class GameLayer : public Layer
{
private:
    GameSession m_Session;

    std::unique_ptr<PaddleSystem> m_PaddleSystem;
    std::unique_ptr<PhysicsSystem> m_PhysicsSystem;

    std::unique_ptr<IGameState> m_CurrentState;

public:

    GameLayer() : Layer("GameLayer") {}

    void OnAttach() override
    {
        m_PaddleSystem = std::make_unique<PaddleSystem>();
        m_PhysicsSystem = std::make_unique<PhysicsSystem>();

        // 2. Creem l'estat inicial i li injectem les dependències
        m_CurrentState = std::make_unique<PlayingState>(&m_Session, m_PaddleSystem.get(), m_PhysicsSystem.get());

        // 3. Arranquem la màquina!
        m_CurrentState->OnEnter();
    }

    void OnUpdate(Timestep ts) override
    {
        if (m_CurrentState) {
            m_CurrentState->OnUpdate(ts);
            m_CurrentState->OnRender();
        }
    }
};
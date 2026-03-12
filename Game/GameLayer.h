#include "EngineCore/Layer.h"

#include "GameCore/GameSession.h"

#include "GameCore/Systems/PaddleSystem.h"
#include "GameCore/Systems/PhysicsSystem.h"

#include "GameCore/States/TitleScreenState.h"
#include "GameCore/States/MainMenuState.h"
#include "GameCore/States/PlayingState.h"
#include "GameCore/States/GameOverState.h"

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

        ChangeState(GameStateType::TitleScreen);
    }

    void ChangeState(GameStateType newState) {

        if (m_CurrentState) {
            m_CurrentState->OnExit();
        }

        switch (newState) {
        case GameStateType::TitleScreen:
            m_CurrentState = std::make_unique<TitleScreenState>();
            break;
        case GameStateType::MainMenu:
            m_CurrentState = std::make_unique<MainMenuState>();
            break;
        case GameStateType::Playing:
            m_CurrentState = std::make_unique<PlayingState>(&m_Session, m_PaddleSystem.get(), m_PhysicsSystem.get());
            break;
        case GameStateType::GameOver:
            m_CurrentState = std::make_unique<GameOverState>(&m_Session);
            break;
        }

        m_CurrentState->SetStateChangeCallback([this](GameStateType state) {
            this->ChangeState(state);
            });

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
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

    // (Fade In / Fade Out)
    bool m_IsTransitioning = false;
    bool m_IsFadingOut = false; // True = To Black, False = To "Transparent"
    float m_TransitionAlpha = 0.0f;
    float m_TransitionSpeed = 2.0f;

    GameStateType m_PendingState;          // Next State, IGameState Callback 1
    std::function<void()> m_PendingAction; // Action to execute when fade in ends, IGameState Callback 2

public:

    GameLayer() : Layer("GameLayer") {}

    void OnAttach() override
    {
        m_PaddleSystem = std::make_unique<PaddleSystem>();
        m_PhysicsSystem = std::make_unique<PhysicsSystem>();

        ChangeState(GameStateType::TitleScreen);
    }

    void ChangeState(GameStateType newState) {
        if (m_IsTransitioning) return; // if still doing fade return

        // Game just opened
        if (!m_CurrentState) {
            ExecuteStateChange(newState);
            return;
        }

        m_PendingState = newState;
        m_IsTransitioning = true;
        m_IsFadingOut = true;
        m_TransitionAlpha = 0.0f;
    }

    void ExecuteStateChange(GameStateType newState) {

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

        m_CurrentState->SetStateChangeCallback(
            [this](GameStateType state) 
            {
                this->ChangeState(state);
            }
        );

        m_CurrentState->SetTransitionCallback(
            [this](std::function<void()> action) {
                if (m_IsTransitioning) return;
                m_PendingAction = action;
                m_IsTransitioning = true;
                m_IsFadingOut = true;
                m_TransitionAlpha = 0.0f;
            }
        );

        m_CurrentState->OnEnter();
    }

    void OnUpdate(Timestep ts) override
    {
        if (m_IsTransitioning) {
            if (m_IsFadingOut) {
                // Fade in
                m_TransitionAlpha += m_TransitionSpeed * ts.GetSeconds();
                if (m_TransitionAlpha >= 1.0f) {
                    m_TransitionAlpha = 1.0f;

                    if (m_PendingAction) {
                        m_PendingAction(); // Execute action
                        m_PendingAction = nullptr; // Clear
                    }
                    else {
                        ExecuteStateChange(m_PendingState); // Change state
                    }

                    m_IsFadingOut = false;
                }
            }
            else {
                // Fade out
                m_TransitionAlpha -= m_TransitionSpeed * ts.GetSeconds();
                if (m_TransitionAlpha <= 0.0f) {
                    m_TransitionAlpha = 0.0f;
                    m_IsTransitioning = false;
                }
            }
        }

        if (m_CurrentState) {
            m_CurrentState->OnUpdate(ts);
        }
    }

    void OnRender() override {
        if (m_CurrentState) {
            m_CurrentState->OnRender();
        }

        // Draw transition above
        if (m_IsTransitioning) {
            Renderer::BeginScene(glm::mat4(1.0f));
            Renderer::DrawQuad({ 0.0f, 0.0f }, { 4.0f, 4.0f }, { 0.0f, 0.0f, 0.0f, m_TransitionAlpha });
            Renderer::EndScene();
        }
    }
};
#include "EngineCore/Layer.h"

#include "GameCore/GameSession.h"

#include "GameCore/Systems/PaddleSystem.h"
#include "GameCore/Systems/PhysicsSystem.h"
#include "GameCore/Systems/BrickSystem.h"
#include "GameCore/Systems/GameFlowSystem.h"

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
	std::unique_ptr<BrickSystem> m_BrickSystem;
	std::unique_ptr<GameFlowSystem> m_GameFlowSystem;

    std::unique_ptr<IGameState> m_CurrentState;

    // Screen transition state
    bool m_IsTransitioning = false;
    bool m_IsFadingToBlack = false; // True = Fading to solid black, False = Fading to transparent
    float m_TransitionAlpha = 0.0f;
    float m_TransitionSpeed = 2.0f;

    GameStateType m_PendingState;          // Next State to load (IGameState Callback 1)
    std::function<void()> m_PendingAction; // Action to execute when fade to black ends (IGameState Callback 2)

public:

    GameLayer() : Layer("GameLayer") {}

    void OnAttach() override
    {
        std::cout << "[GameLayer] Attaching to application. Initializing core systems...\n";

        m_PaddleSystem = std::make_unique<PaddleSystem>();
        m_PhysicsSystem = std::make_unique<PhysicsSystem>();
        m_BrickSystem = std::make_unique<BrickSystem>();
        m_GameFlowSystem = std::make_unique<GameFlowSystem>();

        ChangeState(GameStateType::TitleScreen);
    }

    void ChangeState(GameStateType newState) {
        if (m_IsTransitioning) return; // Ignore if a transition is already in progress

        // Immediate transition if the game just opened (no previous state)
        if (!m_CurrentState) {
            ExecuteStateChange(newState);
            return;
        }

        std::cout << "[GameLayer] Requesting state change. Starting screen fade...\n";

        m_PendingState = newState;
        m_IsTransitioning = true;
        m_IsFadingToBlack = true;
        m_TransitionAlpha = 0.0f;
    }

    void ExecuteStateChange(GameStateType newState) {

        if (m_CurrentState) {
            m_CurrentState->OnExit();
        }

        switch (newState) {
        case GameStateType::TitleScreen:
            std::cout << "[GameLayer] Loading TitleScreenState\n";
            m_CurrentState = std::make_unique<TitleScreenState>();
            break;
        case GameStateType::MainMenu:
            std::cout << "[GameLayer] Loading MainMenuState\n";
            m_CurrentState = std::make_unique<MainMenuState>(&m_Session);   
            break;
        case GameStateType::Playing:
            std::cout << "[GameLayer] Loading PlayingState\n";
            m_CurrentState = std::make_unique<PlayingState>(&m_Session, m_PaddleSystem.get(), m_PhysicsSystem.get(), 
                m_BrickSystem.get(), m_GameFlowSystem.get());
            break;
        case GameStateType::GameOver:
            std::cout << "[GameLayer] Loading GameOverState\n";
            m_CurrentState = std::make_unique<GameOverState>(&m_Session);
            break;
        }

        // Hook up the state change callback
        m_CurrentState->SetStateChangeCallback(
            [this](GameStateType state) 
            {
                this->ChangeState(state);
            }
        );

        // Hook up the custom transition action callback
        m_CurrentState->SetTransitionCallback(
            [this](std::function<void()> action) {
                if (m_IsTransitioning) return;
                m_PendingAction = action;
                m_IsTransitioning = true;
                m_IsFadingToBlack = true;
                m_TransitionAlpha = 0.0f;
            }
        );

        m_CurrentState->OnEnter();
    }

    void OnUpdate(Timestep ts) override
    {
        if (m_IsTransitioning) {
            if (m_IsFadingToBlack) {
                // Fade to black
                m_TransitionAlpha += m_TransitionSpeed * ts.GetSeconds();
                if (m_TransitionAlpha >= 1.0f) {
                    m_TransitionAlpha = 1.0f;

                    if (m_PendingAction) {
                        m_PendingAction(); // Execute custom action (e.g., load next level)
                        m_PendingAction = nullptr; // Clear callback
                    }
                    else {
                        ExecuteStateChange(m_PendingState); // Load the pending state
                    }

                    m_IsFadingToBlack = false; // Start fading back to clear
                }
            }
            else {
                // Fade to clear
                m_TransitionAlpha -= m_TransitionSpeed * ts.GetSeconds();
                if (m_TransitionAlpha <= 0.0f) {
                    m_TransitionAlpha = 0.0f;
					m_IsTransitioning = false; // Transition complete
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

        // Draw global fade transition on top of everything
        if (m_IsTransitioning) {
            Renderer::BeginScene(glm::mat4(1.0f));
            Renderer::DrawQuad({ 0.0f, 0.0f }, { 4.0f, 4.0f }, { 0.0f, 0.0f, 0.0f, m_TransitionAlpha });
            Renderer::EndScene();
        }
    }
};
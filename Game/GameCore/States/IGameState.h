#pragma once
#include "EngineCore/Timestep.h"

// Defines the available macro-states of the game application
enum class GameStateType {
    TitleScreen,
    MainMenu,
    Options, // Kept for compatibility if ever decide to add it back
    Playing,
    GameOver
};

// Base interface for all game states. Follows the State Design Pattern.
class IGameState {
protected:
    // CALLBACK 1: STATE CHANGE
    // Behavior: Fade Out -> Delete Old State -> Create New State -> Fade In
    // Use this to transition completely from one GameState to another.
    std::function<void(GameStateType)> m_RequestStateChange;        

    // CALLBACK 2: TRANSITION
    // Behavior: Fade Out -> Execute custom lambda function -> Fade In
    // Use this to mask background loading or level resets without changing the active state.
    std::function<void(std::function<void()>)> m_RequestTransition; 

public:
    virtual ~IGameState() = default;

    // Registers the callback provided by the orchestrator (GameLayer)
    void SetStateChangeCallback(const std::function<void(GameStateType)>& callback) {
        m_RequestStateChange = callback;
    }

    // Registers the callback provided by the orchestrator (GameLayer)
    void SetTransitionCallback(const std::function<void(std::function<void()>)>& callback) {
        m_RequestTransition = callback;
    }

    virtual void OnEnter() {}
    virtual void OnUpdate(Timestep ts) = 0;
    virtual void OnRender() = 0;
    virtual void OnExit() {}
};
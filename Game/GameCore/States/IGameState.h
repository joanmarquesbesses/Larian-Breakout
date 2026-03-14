#pragma once
#include "EngineCore/Timestep.h"

enum class GameStateType {
    TitleScreen,
    MainMenu,
    Playing,
    GameOver
};

class IGameState {
protected:
    // CALLBACK 1: STATE CHANGE
    // Fade Out -> Delete Old State -> Create New State -> Fade In
    std::function<void(GameStateType)> m_RequestStateChange;        

    // CALLBACK 2: TRANSITION
    // Fade Out -> Execute function (eg: load new blocks) -> Fade In
    // Doesn't change state
    std::function<void(std::function<void()>)> m_RequestTransition; 

public:
    virtual ~IGameState() = default;

    void SetStateChangeCallback(const std::function<void(GameStateType)>& callback) {
        m_RequestStateChange = callback;
    }

    void SetTransitionCallback(const std::function<void(std::function<void()>)>& callback) {
        m_RequestTransition = callback;
    }

    virtual void OnEnter() {}
    virtual void OnUpdate(Timestep ts) = 0;
    virtual void OnRender() = 0;
    virtual void OnExit() {}
};
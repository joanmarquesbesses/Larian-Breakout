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
    std::function<void(GameStateType)> m_RequestStateChange;

public:
    virtual ~IGameState() = default;

    void SetStateChangeCallback(const std::function<void(GameStateType)>& callback) {
        m_RequestStateChange = callback;
    }

    virtual void OnEnter() {}
    virtual void OnUpdate(Timestep ts) = 0;
    virtual void OnRender() = 0;
    virtual void OnExit() {}
};
#pragma once
#include "EngineCore/Timestep.h"

class IGameState {
public:
    virtual ~IGameState() = default;
    virtual void OnEnter() {}
    virtual void OnUpdate(Timestep ts) = 0;
    virtual void OnRender() = 0;
    virtual void OnExit() {}
};
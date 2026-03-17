#pragma once

#include <string>
#include "Timestep.h"
#include "Events/Event.h" 

// Base class for an application layer. Layers can be attached/detached and receive updates/events.
class Layer
{
public:
    Layer(const std::string& name = "Layer") : m_DebugName(name) {}
    virtual ~Layer() = default;

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(Timestep ts) {}
    virtual void OnRender() {}
    virtual void OnEvent(Event& event) {}

    inline const std::string& GetName() const { return m_DebugName; }
protected:
    std::string m_DebugName;
};
#pragma once

#include <string>
#include <memory>

#include "EngineCore/Window.h"
#include "EngineCore/LayerStack.h"
#include "Events/ApplicationEvent.h"
#include "EngineCore/Timestep.h"

// The core engine application class. Manages the main game loop, 
// window events, and the layer stack.
class Application
{
public:
    Application(const std::string& name = "Larian Breakout - Technical Showcase");
    virtual ~Application();

    void Run();
    void Close();
    void OnEvent(Event& e);

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* overlay);

    inline static Application& Get() { return *s_Instance; }
    inline Window& GetWindow() { return *m_Window; }

    float GetAverageFPS() const { return m_AverageFPS; }

private:
    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);

private:
    std::unique_ptr<Window> m_Window;
    bool m_Running = true;
    bool m_Minimized = false;

    LayerStack m_LayerStack;

    // Time management
    float m_LastFrameTime = 0.0f;
    float m_FrameTimeAccumulator = 0.0f;
    int m_FrameCount = 0;
    float m_AverageFPS = 0.0f;

private:
    static Application* s_Instance;
};

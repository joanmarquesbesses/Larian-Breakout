#pragma once

#include <string>
#include <memory>
#include <vector>

#include "Window.h"
#include "LayerStack.h"
#include "Events/ApplicationEvent.h"
#include "Timestep.h"

class Application
{
public:
    Application(const std::string& name = "Larian Arkanoid");
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

    // Control de temps
    float m_LastFrameTime = 0.0f;
    float m_FrameTimeAccumulator = 0.0f;
    int m_FrameCount = 0;
    float m_AverageFPS = 0.0f;

private:
    static Application* s_Instance;
};

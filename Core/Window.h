#pragma once

#include <string>
#include <functional>
#include <memory>
#include <GLFW/glfw3.h>
#include "Events/Event.h"

struct WindowProps {
    std::string Title;
    uint32_t Width;
    uint32_t Height;

    WindowProps(const std::string& title = "Larian Arkanoid", uint32_t width = 1280, uint32_t height = 720)
        : Title(title), Width(width), Height(height) {
    }
};

class Window {
public:
    using EventCallbackFn = std::function<void(Event&)>;

    Window(const WindowProps& props);
    ~Window();

    void OnUpdate();

    uint32_t GetWidth() const { return m_Data.Width; }
    uint32_t GetHeight() const { return m_Data.Height; }

    void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
    void SetVSync(bool enabled);
    bool IsVSync() const { return m_Data.VSync; }

    GLFWwindow* GetNativeWindow() const { return m_Window; }

    static Window* Create(const WindowProps& props = WindowProps());

private:
    void Init(const WindowProps& props);
    void Shutdown();

private:
    GLFWwindow* m_Window;

    struct WindowData {
        std::string Title;
        uint32_t Width, Height;
        bool VSync;
        EventCallbackFn EventCallback;
    };

    WindowData m_Data;
};
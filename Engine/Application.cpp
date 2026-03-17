#include "Application.h"
#include <iostream>
#include <GLFW/glfw3.h>

#include "Assets/ResourceManager.h"
#include "Renderer/Renderer.h"
#include "Utils/Random.h"
#include "Audio/AudioEngine.h"

Application* Application::s_Instance = nullptr;

Application::Application(const std::string& name)
{
    if (s_Instance) {
        std::cout << "[Engine] ERROR: Application instance already exists!\n";
        return;
    }
    s_Instance = this;

    std::cout << "[Engine] Initializing Application instance...\n";

    // Create the window and set its event callback
    m_Window = std::unique_ptr<Window>(Window::Create(WindowProps(name, 1280, 720)));
    m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

    std::cout << "[Engine] Initializing core subsystems (Random, Renderer, Audio)...\n";

    // Initialize core subsystems
    Random::Init();
    Renderer::Init();
    AudioEngine::Init();
}

Application::~Application()
{
    std::cout << "[Engine] Shutting down Application and destroying layers...\n";
    m_LayerStack.Clear();

    std::cout << "[Engine] Shutting down core subsystems...\n";
    m_Window.reset();
    Renderer::Shutdown();
    AudioEngine::Shutdown();
    ResourceManager::Clear();
}

void Application::PushLayer(Layer* layer)
{
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
}

void Application::PushOverlay(Layer* overlay)
{
    m_LayerStack.PushOverlay(overlay);
    overlay->OnAttach();
}

void Application::Close()
{
    std::cout << "[Engine] Application close requested.\n";
    m_Running = false;
}

void Application::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
    dispatcher.Dispatch<WindowResizeEvent>(std::bind(&Application::OnWindowResize, this, std::placeholders::_1));

    // Propagate events through the layer stack (from top overlay down to base layer)
    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
        (*--it)->OnEvent(e);
        if (e.IsHandled())
            break;
    }
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
    m_Running = false;
    return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e)
{
    if (e.GetWidth() == 0 || e.GetHeight() == 0) {
        m_Minimized = true;
        return false;
    }

    m_Minimized = false;
    Renderer::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
    return false;
}

void Application::Run()
{
    std::cout << "[Engine] Entering main game loop...\n";

    while (m_Running) {

        float time = (float)glfwGetTime();
        Timestep timestep = time - m_LastFrameTime;
        m_LastFrameTime = time;

        // FPS Calculation
        m_FrameTimeAccumulator += timestep.GetSeconds();
        m_FrameCount++;
        if (m_FrameTimeAccumulator >= 1.0f) {
            m_AverageFPS = m_FrameCount / m_FrameTimeAccumulator;
            m_FrameTimeAccumulator = 0.0f;
            m_FrameCount = 0;
        }

        // Only update and render if the window is not minimized
        if (!m_Minimized) {
            for (Layer* layer : m_LayerStack) {
                layer->OnUpdate(timestep);
            }
            for (Layer* layer : m_LayerStack) {
                layer->OnRender();
            }
        }

        m_Window->OnUpdate();
    }
}
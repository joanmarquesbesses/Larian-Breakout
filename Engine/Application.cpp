#include "Application.h"
#include <iostream>
#include <GLFW/glfw3.h>

#include "Assets/ResourceManager.h"
#include "Renderer/Renderer.h"
#include "Utils/Random.h"
// #include "AudioEngine.h"

Application* Application::s_Instance = nullptr;

Application::Application(const std::string& name)
{
    if (s_Instance) {
        std::cout << "ERROR: L'Application already exist!\n";
        return;
    }
    s_Instance = this;

    // Creem la finestra i li diem on enviar els events
    m_Window = std::unique_ptr<Window>(Window::Create(WindowProps(name, 1280, 720)));
    m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

    // Aquí aniran els Inits dels teus sistemes (quan els portis)
    Random::Init();
    Renderer::Init();
    // AudioEngine::Init();
}

Application::~Application()
{
    m_LayerStack.Clear();

    m_Window.reset();
    Renderer::Shutdown();
    // AudioEngine::Shutdown();
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
    m_Running = false;
}

void Application::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
    dispatcher.Dispatch<WindowResizeEvent>(std::bind(&Application::OnWindowResize, this, std::placeholders::_1));

    // Propaguem l'event per les capes (des de dalt cap a baix)
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
    while (m_Running) {

        float time = (float)glfwGetTime();
        Timestep timestep = time - m_LastFrameTime;
        m_LastFrameTime = time;

        m_FrameTimeAccumulator += timestep.GetSeconds();
        m_FrameCount++;
        if (m_FrameTimeAccumulator >= 1.0f) {
            m_AverageFPS = m_FrameCount / m_FrameTimeAccumulator;
            m_FrameTimeAccumulator = 0.0f;
            m_FrameCount = 0;
        }

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
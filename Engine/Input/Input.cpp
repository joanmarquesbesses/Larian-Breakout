#include "Input.h"
#include "Application.h"
#include <GLFW/glfw3.h>

bool Input::IsKeyPressed(KeyCode keycode)
{
    // Fixa't com agafa la finestra directament del nostre Application
    auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetKey(window, static_cast<int>(keycode));
    return (state == GLFW_PRESS || state == GLFW_REPEAT);
}

bool Input::IsMouseButtonPressed(const MouseButton button)
{
    auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetMouseButton(window, static_cast<int>(button));
    return (state == GLFW_PRESS);
}

glm::vec2 Input::GetMousePosition()
{
    auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return { static_cast<float>(xpos), static_cast<float>(ypos) };
}

float Input::GetMouseX()
{
    return GetMousePosition().x;
}

float Input::GetMouseY()
{
    return GetMousePosition().y;
}
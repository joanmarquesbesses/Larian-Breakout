#pragma once

#include "KeyCodes.h"
#include "MouseButtonCodes.h"
#include <glm/glm.hpp>

class Input
{
public:
    static bool IsKeyPressed(KeyCode keycode);
    static bool IsMouseButtonPressed(MouseButton button);

    static glm::vec2 GetMousePosition();
    static float GetMouseX();
    static float GetMouseY();
};
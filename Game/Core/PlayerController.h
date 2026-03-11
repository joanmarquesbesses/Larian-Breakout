#pragma once

#include "Input/Input.h"
#include <optional>

enum class PlayerAction {
    MoveLeft, MoveRight, Fire, Pause
};

class PlayerController {
public:
    static std::optional<PlayerAction> GetPaddleAction() {
        // Todo: Gamepad

        if (Input::IsKeyPressed(KeyCode::A) || Input::IsKeyPressed(KeyCode::Left)) {
            return PlayerAction::MoveLeft;
        }
        else if (Input::IsKeyPressed(KeyCode::D) || Input::IsKeyPressed(KeyCode::Right)) {
            return PlayerAction::MoveRight;
        }

        return std::nullopt;
    }

    static bool IsActionPressed(PlayerAction action) {
        if (action == PlayerAction::Fire) return Input::IsKeyPressed(KeyCode::Space);
        if (action == PlayerAction::Pause) return Input::IsKeyPressed(KeyCode::Escape);
        return false;
    }
};
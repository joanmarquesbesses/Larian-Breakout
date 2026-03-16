#pragma once

#include "Input/Input.h"
#include <optional>

enum class PlayerAction {
    MoveLeft, MoveRight, Fire, Pause,
    Up, Down, Accept, Left, Right
};

class PlayerController {
private:
    inline static bool s_FireConsumed = false;
    inline static bool s_AcceptConsumed = false;
    inline static bool s_UpConsumed = false;
    inline static bool s_DownConsumed = false;
    inline static bool s_LeftConsumed = false;  
    inline static bool s_RightConsumed = false;
    inline static bool s_PauseConsumed = false;

public:
    static void ConsumeInput(PlayerAction action) {
        if (action == PlayerAction::Accept) s_AcceptConsumed = true;
        if (action == PlayerAction::Up) s_UpConsumed = true;
        if (action == PlayerAction::Down) s_DownConsumed = true;
        if (action == PlayerAction::Left) s_LeftConsumed = true;   
        if (action == PlayerAction::Right) s_RightConsumed = true; 
        if (action == PlayerAction::Fire) s_FireConsumed = true;
        if (action == PlayerAction::Pause) s_PauseConsumed = true;
    }

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
        if (action == PlayerAction::Accept) {
            bool isPressed = Input::IsKeyPressed(KeyCode::Enter);
            if (!isPressed) s_AcceptConsumed = false;
            return isPressed && !s_AcceptConsumed;
        }

        if (action == PlayerAction::Up) {
            bool isPressed = Input::IsKeyPressed(KeyCode::W) || Input::IsKeyPressed(KeyCode::Up);
            if (!isPressed) s_UpConsumed = false;
            return isPressed && !s_UpConsumed;
        }

        if (action == PlayerAction::Down) {
            bool isPressed = Input::IsKeyPressed(KeyCode::S) || Input::IsKeyPressed(KeyCode::Down);
            if (!isPressed) s_DownConsumed = false;
            return isPressed && !s_DownConsumed;
        }

        if (action == PlayerAction::Left) {
            bool isPressed = Input::IsKeyPressed(KeyCode::A) || Input::IsKeyPressed(KeyCode::Left);
            if (!isPressed) s_LeftConsumed = false;
            return isPressed && !s_LeftConsumed;
        }

        if (action == PlayerAction::Right) {
            bool isPressed = Input::IsKeyPressed(KeyCode::D) || Input::IsKeyPressed(KeyCode::Right);
            if (!isPressed) s_RightConsumed = false;
            return isPressed && !s_RightConsumed;
        }

        if (action == PlayerAction::Fire) {
            bool isPressed = Input::IsKeyPressed(KeyCode::Space);
            if (!isPressed) s_FireConsumed = false;
            return isPressed && !s_FireConsumed;
        }

        if (action == PlayerAction::Pause) {
            bool isPressed = Input::IsKeyPressed(KeyCode::Escape);
            if (!isPressed) s_PauseConsumed = false;
            return isPressed && !s_PauseConsumed;
        }

        if (action == PlayerAction::Pause) return Input::IsKeyPressed(KeyCode::Escape);

        return false;
    }

    static bool ConsumeIfPressed(PlayerAction action) {
        if (IsActionPressed(action)) {
            ConsumeInput(action);
            return true;
        }
        return false;
    }
};
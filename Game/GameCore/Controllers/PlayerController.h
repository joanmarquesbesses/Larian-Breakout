#pragma once

#include "Input/Input.h"
#include <optional>

// Maps specific keys to logical game actions to decouple input devices from gameplay logic.
enum class PlayerAction {
    MoveLeft, MoveRight, Fire, Pause,
    Up, Down, Accept, Left, Right, SkipLevel
};

class PlayerController {
private:
    // State flags for input debouncing (ensures single trigger per key press)
    inline static bool s_FireConsumed = false;
    inline static bool s_AcceptConsumed = false;
    inline static bool s_UpConsumed = false;
    inline static bool s_DownConsumed = false;
    inline static bool s_LeftConsumed = false;  
    inline static bool s_RightConsumed = false;
    inline static bool s_PauseConsumed = false;
	inline static bool s_SkipLevelConsumed = false;

public:
    // Marks an action as consumed so it won't trigger again until the key is released
    static void ConsumeInput(PlayerAction action) {
        if (action == PlayerAction::Accept) s_AcceptConsumed = true;
        if (action == PlayerAction::Up) s_UpConsumed = true;
        if (action == PlayerAction::Down) s_DownConsumed = true;
        if (action == PlayerAction::Left) s_LeftConsumed = true;   
        if (action == PlayerAction::Right) s_RightConsumed = true; 
        if (action == PlayerAction::Fire) s_FireConsumed = true;
        if (action == PlayerAction::Pause) s_PauseConsumed = true;
        if (action == PlayerAction::SkipLevel) s_SkipLevelConsumed = true;
    }

    // Continuous input check for paddle movement
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

    // Checks if an action is currently pressed and hasn't been consumed yet
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

        if (action == PlayerAction::SkipLevel) {
            bool isPressed = Input::IsKeyPressed(KeyCode::F1);
            if (!isPressed) s_SkipLevelConsumed = false;
            return isPressed && !s_SkipLevelConsumed;
		}

        return false;
    }

    // Checks if the action is pressed, and if so, consumes it immediately (useful for UI navigation)
    static bool ConsumeIfPressed(PlayerAction action) {
        if (IsActionPressed(action)) {
            ConsumeInput(action);
            return true;
        }
        return false;
    }
};
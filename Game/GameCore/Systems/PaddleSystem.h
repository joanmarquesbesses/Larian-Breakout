#pragma once
#include "GameCore/GameSession.h"
#include "GameCore/Controllers/PlayerController.h"

// Handles the continuous update of the player's paddle based on input
class PaddleSystem {
public:
    void Update(GameSession& session, float dt) {
        // Fetch the current action from the controller and apply movement
        if (auto action = PlayerController::GetPaddleAction()) {
            if (*action == PlayerAction::MoveLeft) {
                session.GetPaddle().MoveLeft(dt);
            }
            else if (*action == PlayerAction::MoveRight) {
                session.GetPaddle().MoveRight(dt);
            }
        }
    }
};
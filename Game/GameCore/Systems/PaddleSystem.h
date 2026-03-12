#pragma once
#include "GameCore/GameSession.h"
#include "GameCore/Controllers/PlayerController.h"

class PaddleSystem {
public:
    void Update(GameSession& session, float dt) {
        if (auto action = PlayerController::GetPaddleAction()) {
            if (*action == PlayerAction::MoveLeft) session.GetPaddle().MoveLeft(dt);
            else if (*action == PlayerAction::MoveRight) session.GetPaddle().MoveRight(dt);
        }
    }
};
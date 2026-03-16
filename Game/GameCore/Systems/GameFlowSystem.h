#pragma once
#include "GameCore/GameSession.h"

enum class FlowState {
    Playing,
    LevelComplete,
    BallLost,
    GameOver
};

class GameFlowSystem {
public:
    FlowState Update(GameSession& session) {
        if (session.GetPlayer().IsDead()) return FlowState::GameOver;

        bool levelComplete = true;
        bool levelCleared = true;

        for (const auto& brick : session.GetCurrentLevel().GetBricks()) {
			if (!brick.IsDestroyed()) levelComplete = false; // If any brick is still alive, the level is not complete
			if (!brick.IsDestroyed() && !brick.IsDying()) levelCleared = false; // If any brick is not destroyed and not dying, the level is not cleared
        }

        if (levelComplete) return FlowState::LevelComplete;

		// If there are no balls in play, we need to check if the player lost a life or if it's game over
        if (session.GetBalls().empty()) {

            // If the level is cleared but there are no balls, we don't want to penalize the player
			if (levelCleared) return FlowState::Playing; 

            session.GetPlayer().LoseLife();

            if (!session.GetPlayer().IsDead()) {
				return FlowState::BallLost; // Player lost a life but still has lives left
            }
            else {
				return FlowState::GameOver; // Player lost a life and has no lives left
            }
        }

		return FlowState::Playing; // No level complete or ball lost conditions met
    }
};
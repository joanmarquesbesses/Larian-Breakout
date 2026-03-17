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
    // Evaluates the current game session and determines the next flow state
    FlowState Update(GameSession& session) {
        if (session.GetPlayer().IsDead()) return FlowState::GameOver;

        bool levelComplete = true;
        bool levelCleared = true;

        for (const auto& brick : session.GetCurrentLevel().GetBricks()) {
            if (brick.GetMaxHealth() == -1) {
                continue; // Ignore indestructible bricks for win condition
            }
			
            // If any destructible brick is still alive, the level is not complete
            if (!brick.IsDestroyed()) {
                levelComplete = false;
            }

            // If any brick is not destroyed and not dying (animating), the level is not cleared yet
            if (!brick.IsDestroyed() && !brick.IsDying()) {
                levelCleared = false;
            }
        }

        if (levelComplete) return FlowState::LevelComplete;

        // Check conditions if there are no balls in play
        if (session.GetBalls().empty()) {

            // If the level is cleared but the last ball fell during the final transition, 
            // we don't penalize the player
			if (levelCleared) return FlowState::Playing; 

            session.GetPlayer().LoseLife();

            if (!session.GetPlayer().IsDead()) {
				return FlowState::BallLost; // Player lost a life but has reserves
            }
            else {
				return FlowState::GameOver; // No lives left
            }
        }

		return FlowState::Playing; // Default state, keep playing
    }
};
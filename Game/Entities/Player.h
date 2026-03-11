#pragma once
#include <string>

struct Player {
    std::string Name = "Player 1";
    int Score = 0;
    int MaxScore = 0;
    int Lives = 3;
    int MaxLevelUnlocked = 1;

    void AddScore(int points) {
        Score += points;
        if (Score > MaxScore) MaxScore = Score;
    }

    void LoseLife() { if (Lives > 0) Lives--; }
    bool IsDead() const { return Lives <= 0; }
};
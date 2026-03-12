#pragma once
#include <string>

class Player {
private:
    std::string m_Name = "Player 1";
    int m_Score = 0;
    int m_MaxScore = 0;
    int m_Lives = 3;
    int m_MaxLevelUnlocked = 1;

public:
    int GetScore() { return m_Score; }

    void AddScore(int points) {
        m_Score += points;
        if (m_Score > m_MaxScore) m_MaxScore = m_Score;
    }

    void LoseLife() { if (m_Lives > 0) m_Lives--; }
    bool IsDead() const { return m_Lives <= 0; }

    void ResetLives() { m_Lives = 3; }
    void ResetScore() { m_Score = 0; }
};
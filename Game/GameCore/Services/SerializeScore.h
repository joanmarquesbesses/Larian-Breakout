#pragma once
#include <fstream>
#include <string>

class SerializeScore {
public:
    static int LoadHighScore() {
        std::ifstream file("highscore.txt");
        int score = 0;
        if (file.is_open()) {
            file >> score;
            file.close();
        }
        return score;
    }

    static bool SaveHighScore(int currentScore) {
        int oldScore = LoadHighScore();

        if (currentScore > oldScore) {
            std::ofstream file("highscore.txt");
            if (file.is_open()) {
                file << currentScore;
                file.close();
            }
            return true;
        }

        return false;
    }
};
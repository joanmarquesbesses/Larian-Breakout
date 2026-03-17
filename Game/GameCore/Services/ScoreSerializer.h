#pragma once
#include <fstream>
#include <string>

// A static service responsible for reading and writing the player's high score to the local disk.
class ScoreSerializer {
public:
    // Reads the highscore.txt file and returns the saved score. Returns 0 if the file doesn't exist.
    static int LoadHighScore() {
        std::ifstream file("highscore.txt");
        int score = 0;
        if (file.is_open()) {
            file >> score;
            file.close();
        }
        return score;
    }

    // Compares the current score with the saved high score. 
    // If the current score is strictly higher, it overwrites the file and returns true.
    static bool SaveHighScore(int currentScore) {
        int oldScore = LoadHighScore();

        if (currentScore > oldScore) {
            std::ofstream file("highscore.txt");
            if (file.is_open()) {
                file << currentScore;
                file.close();
            }
            return true; // New High Score achieved!
        }

        return false; // Score wasn't high enough to save
    }
};
#pragma once
#include "Entities/Level.h"
#include "GameCore/Resources/LevelCatalog.h"

class LevelSetup { 
public:
    static int GetLevelCount() { return LevelCatalog::Levels.size(); }

    static void LoadLevelFromData(Level& level, int levelIndex) {
        level.GetBricks().clear();

        std::vector<glm::vec4> brickColors = {
            { 0.0f, 0.0f, 0.0f, 0.0f }, // 0
            { 0.8f, 0.2f, 0.2f, 1.0f }, // 1 (Red)
            { 0.2f, 0.8f, 0.2f, 1.0f }, // 2 (Green)
            { 0.2f, 0.2f, 0.8f, 1.0f }  // 3 (Blue)
        };

        if (levelIndex < 0 || levelIndex >= LevelCatalog::Levels.size()) levelIndex = 0;

        const auto& currentLevelData = LevelCatalog::Levels[levelIndex];

        int rows = currentLevelData.size();
        int cols = currentLevelData[0].size();

        float startX = level.GetLeftLimit() + 0.1f;  // Add some margin
        float endX = level.GetRightLimit() - 0.1f;
        float startY = level.GetTopLimit() - 0.1f;
        float endY = 0.0f; // Bottom bricks can reach half of the level

        float areaWidth = endX - startX;
        float areaHeight = startY - endY;

        float brickWidth = areaWidth / cols;
        float brickHeight = areaHeight / rows;

        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                char c = currentLevelData[y][x];

                if (c == ' ') continue;

                // Central brick position
                float posX = startX + (x * brickWidth) + (brickWidth / 2.0f);
                float posY = startY - (y * brickHeight) - (brickHeight / 2.0f);

                Brick newBrick({ posX, posY }, { brickWidth * 0.9f, brickHeight * 0.9f });

                int hp = 0;
                int maxHp = 0;
                bool isExplosive = false;
                glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

                if (c >= '1' && c <= '3') {
                    // Normal bricks (1, 2, 3)
                    hp = c - '0';
                    maxHp = hp;
                    color = brickColors[hp];
                }
                else if (c >= 'A' && c <= 'C') {
                    // Bomb bricks (A, B, C)
                    hp = (c - 'A') + 1; // 'A' val 1, 'B' val 2, 'C' val 3
                    maxHp = hp;
                    isExplosive = true;
                    color = brickColors[hp];
                }
                else if (c == 'X' || c == 'x') {
                    // Indestructible bricks  (X)
                    hp = -1;
                    maxHp = -1; 
                    color = { 0.4f, 0.4f, 0.4f, 1.0f }; // Grey
                }
                else {
                    continue; 
                }

                newBrick.SetColor(color);
                newBrick.SetHealth(hp);
                newBrick.SetMaxHealth(maxHp);    
                newBrick.SetExplosive(isExplosive); 

                level.GetBricks().push_back(newBrick);
            }
        }
    }
};
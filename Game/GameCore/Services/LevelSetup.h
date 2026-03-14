#pragma once
#include "Entities/Level.h"
#include "GameCore/Resources/LevelCatalog.h"

class LevelSetup { 
public:
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
                char charType = currentLevelData[y][x];

                if (charType == ' ') continue;

                int hp = charType - '0'; // Eg: 50 in ASCII (2(int)) - 48 in ASCII (0(int)) = 2(int)
                if (hp <= 0 || hp >= brickColors.size()) continue;

                // Central brick position
                float posX = startX + (x * brickWidth) + (brickWidth / 2.0f);
                float posY = startY - (y * brickHeight) - (brickHeight / 2.0f);

                Brick newBrick({ posX, posY }, { brickWidth * 0.9f, brickHeight * 0.9f });
                newBrick.SetColor(brickColors[hp]);

                newBrick.SetHealth(hp); 

                level.GetBricks().push_back(newBrick);
            }
        }
    }
};
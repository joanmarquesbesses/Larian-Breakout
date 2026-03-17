#pragma once
#include "Entities/Level.h"
#include "GameCore/Resources/LevelCatalog.h"

class LevelSetup { 
public:
    static int GetLevelCount() { return LevelCatalog::Levels.size(); }

    // Parses a 2D array of strings into physical Brick entities placed in the game world
    static void LoadLevelFromData(Level& level, int levelIndex) {
        level.GetBricks().clear();

        // Standard palette for brick health (1 hit = Red, 2 hits = Green, 3 hits = Blue)
        std::vector<glm::vec4> brickColors = {
            { 0.0f, 0.0f, 0.0f, 0.0f }, // 0 (Empty/Invisible)
            { 0.8f, 0.2f, 0.2f, 1.0f }, // 1 (Red)
            { 0.2f, 0.8f, 0.2f, 1.0f }, // 2 (Green)
            { 0.2f, 0.2f, 0.8f, 1.0f }  // 3 (Blue)
        };

        // Safety check: loop back to the first level if the index is out of bounds
        if (levelIndex < 0 || levelIndex >= LevelCatalog::Levels.size()) levelIndex = 0;

        const auto& currentLevelData = LevelCatalog::Levels[levelIndex];

        int rows = currentLevelData.size();
        int cols = currentLevelData[0].size();

        // 1. Define the playable area bounds for the bricks
        // Added a 0.1f margin so the bricks don't touch the exact edges of the screen
        float startX = level.GetLeftLimit() + 0.1f;
        float endX = level.GetRightLimit() - 0.1f;
        float startY = level.GetTopLimit() - 0.1f;
        float endY = 0.0f; // Bricks will only spawn in the top half of the screen (above Y=0)

        // 2. Calculate the total size of this bounding box
        float areaWidth = endX - startX;
        float areaHeight = startY - endY;

        // 3. Calculate the size of each individual brick based on the grid dimensions
        float brickWidth = areaWidth / cols;
        float brickHeight = areaHeight / rows;

        // 4. Iterate over the 2D text array (y = rows, x = columns)
        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                char c = currentLevelData[y][x];

                if (c == ' ') continue; // Skip empty spaces

                // 5. Calculate the exact world position for the center of this specific brick
                // X: Start from the left edge + skip 'x' bricks + add half a brick to reach the center
                float posX = startX + (x * brickWidth) + (brickWidth / 2.0f);
                // Y: Start from the top edge - skip 'y' bricks - subtract half a brick to reach the center (Y goes down)
                float posY = startY - (y * brickHeight) - (brickHeight / 2.0f);

                // Create the brick, scaling it down slightly (0.9f) to leave a small gap between them
                Brick newBrick({ posX, posY }, { brickWidth * 0.9f, brickHeight * 0.9f });

                int hp = 0;
                int maxHp = 0;
                bool isExplosive = false;
                glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

                // 6. Parse the character to apply game logic
                if (c >= '1' && c <= '3') {
                    // Normal destructible bricks (Math trick: '2' - '0' = integer 2)
                    hp = c - '0';
                    maxHp = hp;
                    color = brickColors[hp];
                }
                else if (c >= 'A' && c <= 'C') {
                    // Explosive bomb bricks (Math trick: 'B' - 'A' = 1. Add 1 to get health 2)
                    hp = (c - 'A') + 1; 
                    maxHp = hp;
                    isExplosive = true;
                    color = brickColors[hp];
                }
                else if (c == 'X' || c == 'x') {
                    // Indestructible walls
                    hp = -1;
                    maxHp = -1; 
                    color = { 0.4f, 0.4f, 0.4f, 1.0f };
                }
                else {
                    continue; // Unknown character fallback
                }

                // Apply the parsed data to our entity and store it in the level
                newBrick.SetColor(color);
                newBrick.SetHealth(hp);
                newBrick.SetMaxHealth(maxHp);    
                newBrick.SetExplosive(isExplosive); 

                level.GetBricks().push_back(newBrick);
            }
        }
    }
};
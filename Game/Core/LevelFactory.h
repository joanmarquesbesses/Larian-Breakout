#pragma once
#include "Entities/Level.h"

class LevelFactory {
public:
    static void GenerateBasicLevel(Level& level) {
        level.GetBricks().clear(); 

        int rows = 5;
        int cols = 10;

        float brickWidth = 0.28f;
        float brickHeight = 0.1f;
        float spacingX = 0.02f;
        float spacingY = 0.02f;

        // Calculem on començar perquè quedi centrat a dalt
        float startX = -((cols * (brickWidth + spacingX)) / 2.0f) + (brickWidth / 2.0f);
        float startY = 0.8f;

        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                float posX = startX + col * (brickWidth + spacingX);
                float posY = startY - row * (brickHeight + spacingY);

                // Creem el totxo. Més endavant aquí decidirem si li donem un PowerUp!
                level.AddBrick(Brick({ posX, posY }, { brickWidth, brickHeight }, 1));
            }
        }
    }
};
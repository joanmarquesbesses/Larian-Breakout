#pragma once
#include "Entities/Level.h"

class LevelFactory { //TODO: buscar un millor nom. ex: levelinitializer 
    //TODO: ser mes coherent amb els noms
public:
    static void SetUpLevel(Level& level,float spaceX = 0.02f, float spaceY = 0.02f) {
        level.GetBricks().clear(); 

        int rows = level.GetRows();
        int cols = level.GetCols();

        float brickWidth = 0.28f;
        float brickHeight = 0.1f;

        float startX = -((cols * (brickWidth + spaceX)) / 2.0f) + (brickWidth / 2.0f);
        float startY = 0.8f;

        glm::vec4 rowColors[] = {
            { 0.8f, 0.2f, 0.2f, 1.0f }, 
            { 0.8f, 0.5f, 0.2f, 1.0f }, 
            { 0.8f, 0.8f, 0.2f, 1.0f }, 
            { 0.2f, 0.8f, 0.2f, 1.0f }, 
            { 0.2f, 0.2f, 0.8f, 1.0f }  
        };
        int numColors = sizeof(rowColors) / sizeof(rowColors[0]);

        for (int row = 0; row < rows; ++row) {
            glm::vec4 color = rowColors[row % numColors];

            for (int col = 0; col < cols; ++col) {
                float posX = startX + col * (brickWidth + spaceX);
                float posY = startY - row * (brickHeight + spaceY);

                level.AddBrick(Brick({ posX, posY }, { brickWidth, brickHeight }, 1, color));
            }
        }
    }
};
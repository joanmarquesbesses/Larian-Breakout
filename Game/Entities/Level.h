#pragma once
#include <vector>

#include "Brick.h"

class Level {
private:
    std::vector<Brick> m_Bricks;
    float m_LeftLimit;   
    float m_RightLimit;  
    float m_TopLimit;    
    float m_BottomLimit;

public:
    Level(float left, float right, float top, float bottom)
        : m_LeftLimit(left), m_RightLimit(right), m_TopLimit(top), m_BottomLimit(bottom) {}

    void AddBrick(const Brick& brick) {
        m_Bricks.push_back(brick);
    }

    std::vector<Brick>& GetBricks() { return m_Bricks; }
    const std::vector<Brick>& GetBricks() const { return m_Bricks; }

    float GetLeftLimit() const { return m_LeftLimit; }
    float GetRightLimit() const { return m_RightLimit; }
    float GetTopLimit() const { return m_TopLimit; }
    float GetBottomLimit() const { return m_BottomLimit; }
};
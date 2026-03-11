#include <vector>
#include <glm/glm.hpp>
#include <optional>
#include <iostream>

#include "Application.h"

class Ball {
private:
    glm::vec2 m_Position;
    glm::vec2 m_Velocity;
    float m_Radius;

public:
    Ball(glm::vec2 startPos, float radius) 
        : m_Position(startPos), m_Velocity(0.0f, 0.0f), m_Radius(radius) {}

    void Move(float deltaTime) {
        m_Position += m_Velocity * deltaTime;
    }

    void Launch(glm::vec2 initialVelocity) {
        m_Velocity = initialVelocity;
    }
};

enum class PowerUpType {
    None = 0,
    ExtraLife,
    ExpandPaddle,
    MultiBall
};

class Brick {
private:
    glm::vec2 m_Position;
    glm::vec2 m_Size;
    int m_Health;
    std::optional<PowerUpType> m_Reward;

public:
    Brick(glm::vec2 pos, glm::vec2 size, int health, PowerUpType powerUp = PowerUpType::None)
        : m_Position(pos), m_Size(size), m_Health(health), m_Reward(powerUp) {}

    void TakeDamage() {
        if (m_Health > 0) m_Health--;
    }

    bool IsDestroyed() const { return m_Health <= 0; }

    std::optional<PowerUpType> TryExtractPowerUp() {
        auto reward = m_Reward;
        m_Reward = std::nullopt;
        return reward;
    }
};

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

class Paddle {
private:
    glm::vec2 m_Position;
    glm::vec2 m_Size;
    float m_Speed;

public:
    Paddle(glm::vec2 startPos, glm::vec2 size, float speed)
        : m_Position(startPos), m_Size(size), m_Speed(speed) {
    }

    void MoveLeft(float dt) { m_Position.x -= m_Speed * dt; }
    void MoveRight(float dt) { m_Position.x += m_Speed * dt; }
};

enum class GameState {
    MainMenu,
    Playing,
    Paused,
    GameOver,
    Victory
};

class GameSession {
private:
    int m_Score = 0;
    int m_Lives = 3;
    GameState m_CurrentState = GameState::MainMenu;

public:
    void AddScore(int score) {
        m_Score += score;
    }

    void LoseLife() {
        --m_Lives;
    }

    void ChangeGameState(GameState gameState) {
        m_CurrentState = m_CurrentState;
    }

    GameState GetGameState() const {
        return m_CurrentState;
    }
};

class ArkanoidApp : public Application
{
public:
    ArkanoidApp() : Application("Larian Arkanoid - Prova Tecnica")
    {
        // Aquí és on més endavant farem:
        // PushLayer(new GameLayer());
        std::cout << "[ArkanoidApp] Joc inicialitzat correctament.\n";
    }

    ~ArkanoidApp()
    {
        std::cout << "[ArkanoidApp] Joc tancat.\n";
    }
};

int main() {

    std::cout << "--- Iniciant Motor ---\n";

    // Instanciem l'aplicació
    ArkanoidApp* app = new ArkanoidApp();

    // Executem el bucle infinit (aquí dins hi ha el Update i el Render)
    app->Run();

    // Neteja de memòria un cop tanquem la finestra
    delete app;

    std::cout << "--- Sortida Neta ---\n";
    return 0;
}
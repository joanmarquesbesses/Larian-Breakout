#include <iostream>
#include <vector>
#include <glm/glm.hpp> // Si això no dóna error subratllat en vermell, Premake ha triomfat!

// 1. EL DOMINI (Només Dades)
struct Ball {
    glm::vec2 Position = { 0.0f, 0.0f };
    glm::vec2 Velocity = { 0.0f, 0.0f };
    float Radius = 0.5f;
};

struct Level {
    float BottomLimit = -10.0f;
};

struct GameSession {
    int Score = 0;
};

// 2. ELS SERVEIS (Només Lògica)
class MovementService {
public:
    void MoveBall(Ball& ball, float deltaTime) {
        ball.Position += ball.Velocity * deltaTime;
    }
};

// 3. EL BANC DE PROVES
int main() {
    std::cout << "--- Iniciant Arkanoid (DDD Architecture) ---\n";

    GameSession session;
    Level level;
    Ball ball;

    // La bola es mou cap amunt
    ball.Velocity = { 0.0f, 2.0f };

    MovementService movement;
    float dummyDeltaTime = 0.5f; // Mig segon per frame (per veure-ho clar)

    for (int i = 0; i < 5; i++) {
        movement.MoveBall(ball, dummyDeltaTime);
        std::cout << "Frame " << i << " | Posicio Bola Y: " << ball.Position.y << "\n";
    }

    return 0;
}
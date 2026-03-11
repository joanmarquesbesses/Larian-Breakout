#pragma once

#include "Entities/Paddle.h"
#include "Entities/Ball.h"
#include "Entities/Level.h"
#include "Entities/Player.h"

class PhysicsService { //stateless
public:
    static void Update(Paddle& paddle, Ball& ball, Level& level, Player& player, float dt) {

        ball.Move(dt);

        glm::vec2 ballSize = { ball.GetSize() * 2.0f, ball.GetSize() * 2.0f };

        auto pos = ball.GetPosition();
        float radius = ball.GetSize();

        // Left Wall
        if (pos.x - radius <= level.GetLeftLimit()) {
            ball.SetPosition({ level.GetLeftLimit() + radius, pos.y }); // La traiem de la paret
            ball.Launch({ std::abs(ball.GetVelocity().x), ball.GetVelocity().y }); // Forcem X positiva
        }

        // Right Wall
        else if (pos.x + radius >= level.GetRightLimit()) {
            ball.SetPosition({ level.GetRightLimit() - radius, pos.y }); // La traiem de la paret
            ball.Launch({ -std::abs(ball.GetVelocity().x), ball.GetVelocity().y }); // Forcem X negativa
        }

        // Top
        if (pos.y + radius >= level.GetTopLimit()) {
            ball.SetPosition({ pos.x, level.GetTopLimit() - radius }); // La baixem una mica
            ball.Launch({ ball.GetVelocity().x, -std::abs(ball.GetVelocity().y) }); // Forcem Y negativa
        }

        // Paddle
        auto paddleHit = GetCollisionNormal(ball.GetPosition(), ballSize, paddle.GetPosition(), paddle.GetSize());
        if (paddleHit.has_value()) {
            glm::vec2 normal = paddleHit.value();
            if (normal.y > 0.0f) { 
                ball.Launch({ ball.GetVelocity().x, std::abs(ball.GetVelocity().y) });
                ball.SetPosition({ ball.GetPosition().x, paddle.GetPosition().y + paddle.GetSize().y / 2.0f + ball.GetSize() });
            }
        }

        // Bricks
        for (auto& brick : level.GetBricks()) {
            if (!brick.IsDestroyed()) {

                auto hitNormal = GetCollisionNormal(ball.GetPosition(), ballSize, brick.GetPosition(), brick.GetSize());

                if (hitNormal.has_value()) {
                    glm::vec2 normal = hitNormal.value();

                    if (normal.x != 0.0f) {
                        ball.Launch({ -ball.GetVelocity().x, ball.GetVelocity().y });
                    }
                    else {
                        ball.Launch({ ball.GetVelocity().x, -ball.GetVelocity().y });
                    }

                    brick.TakeDamage();
                    if (brick.IsDestroyed()) {
                        player.AddScore(100);
                    }

                    break;
                }
            }
        }
    }

private:
    static std::optional<glm::vec2> GetCollisionNormal(glm::vec2 posA, glm::vec2 sizeA, glm::vec2 posB, glm::vec2 sizeB)
    {
        //TODO: saber explicar la idea general de les colisions.

        float dx = posA.x - posB.x;
        float dy = posA.y - posB.y;

        float overlapX = (sizeA.x / 2.0f + sizeB.x / 2.0f) - std::abs(dx);
        float overlapY = (sizeA.y / 2.0f + sizeB.y / 2.0f) - std::abs(dy);

        if (overlapX > 0.0f && overlapY > 0.0f)
        {
            if (overlapX < overlapY) {
                return dx > 0.0f ? glm::vec2(1.0f, 0.0f) : glm::vec2(-1.0f, 0.0f);
            }
            else {
                return dy > 0.0f ? glm::vec2(0.0f, 1.0f) : glm::vec2(0.0f, -1.0f);
            }
        }

        return std::nullopt; 
    }
};
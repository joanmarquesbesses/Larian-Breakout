#pragma once

#include "Entities/Paddle.h"
#include "Entities/Ball.h"
#include "Entities/Level.h"
#include "Entities/Player.h"

class PhysicsService {
public:
    static void Update(Paddle& paddle, Ball& ball, Level& level, Player& player, float dt) {

        // 1. Moviment de la bola
        ball.Move(dt);

        // 2. Col·lisions de la bola amb les parets
        auto pos = ball.GetPosition();
        if (pos.x <= level.GetLeftLimit() || pos.x >= level.GetRightLimit()) {
            ball.Launch({ -ball.GetVelocity().x, ball.GetVelocity().y });
        }
        if (pos.y >= level.GetTopLimit()) {
            ball.Launch({ ball.GetVelocity().x, -ball.GetVelocity().y });
        }

        // 3. Col·lisió amb la Pala
        if (CheckAABB(ball.GetPosition(), { ball.GetSize(), ball.GetSize() }, paddle.GetPosition(), paddle.GetSize())) {
            ball.Launch({ ball.GetVelocity().x, -ball.GetVelocity().y });
            ball.SetPosition({ ball.GetPosition().x, paddle.GetPosition().y + paddle.GetSize().y / 2.0f + ball.GetSize() / 2.0f });
        }

        // 4. Col·lisió amb els Totxos
        for (auto& brick : level.GetBricks()) {
            if (!brick.IsDestroyed()) {
                if (CheckAABB(ball.GetPosition(), { ball.GetSize(), ball.GetSize() }, brick.GetPosition(), brick.GetSize())) {

                    ball.Launch({ ball.GetVelocity().x, -ball.GetVelocity().y });
                    brick.TakeDamage();

                    if (brick.IsDestroyed()) {
                        player.AddScore(100);
                        // Aquí en el futur farem: if(brick.TryExtractPowerUp().has_value()) ...
                    }
                    break;
                }
            }
        }
    }

private:
    static bool CheckAABB(glm::vec2 posA, glm::vec2 sizeA, glm::vec2 posB, glm::vec2 sizeB) {
        bool collisionX = posA.x + sizeA.x / 2.0f >= posB.x - sizeB.x / 2.0f && posB.x + sizeB.x / 2.0f >= posA.x - sizeA.x / 2.0f;
        bool collisionY = posA.y + sizeA.y / 2.0f >= posB.y - sizeB.y / 2.0f && posB.y + sizeB.y / 2.0f >= posA.y - sizeA.y / 2.0f;
        return collisionX && collisionY;
    }
};
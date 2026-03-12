#pragma once
#include "GameCore/GameSession.h"

class PhysicsSystem {
public:

    void Update(GameSession& session, float dt) {

        for (auto& ball : session.GetBalls()) {
            ball.Move(dt);

            glm::vec2 ballSize = { ball.GetSize() * 2.0f, ball.GetSize() * 2.0f };

            auto pos = ball.GetPosition();
            float radius = ball.GetSize();

            // Left Wall
            if (pos.x - radius <= session.GetCurrentLevel().GetLeftLimit()) {
                ball.SetPosition({ session.GetCurrentLevel().GetLeftLimit() + radius, pos.y }); 
                ball.Launch({ std::abs(ball.GetVelocity().x), ball.GetVelocity().y });
            }

            // Right Wall
            else if (pos.x + radius >= session.GetCurrentLevel().GetRightLimit()) {
                ball.SetPosition({ session.GetCurrentLevel().GetRightLimit() - radius, pos.y });
                ball.Launch({ -std::abs(ball.GetVelocity().x), ball.GetVelocity().y });
            }

            // Top
            if (pos.y + radius >= session.GetCurrentLevel().GetTopLimit()) {
                ball.SetPosition({ pos.x, session.GetCurrentLevel().GetTopLimit() - radius });
                ball.Launch({ ball.GetVelocity().x, -std::abs(ball.GetVelocity().y) });
            }

            // Paddle
            auto& paddle = session.GetPaddle();
            auto paddleHit = GetCollisionNormal(ball.GetPosition(), ballSize, paddle.GetPosition(), paddle.GetSize());
            if (paddleHit.has_value()) {
                glm::vec2 normal = paddleHit.value();
                if (normal.y > 0.0f) {
                    ball.Launch({ ball.GetVelocity().x, std::abs(ball.GetVelocity().y) });
                    ball.SetPosition({ ball.GetPosition().x, paddle.GetPosition().y + paddle.GetSize().y / 2.0f + ball.GetSize() });
                }
            }

            // Bricks
            for (auto& brick : session.GetCurrentLevel().GetBricks()) {
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
                            session.GetPlayer().AddScore(100);
                        }

                        break;
                    }
                }
            }

            if (ball.GetPosition().y < session.GetCurrentLevel().GetBottomLimit()) {
                session.GetPlayer().LoseLife();
                session.ToggleIsBallInPlay();
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
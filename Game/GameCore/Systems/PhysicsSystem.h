#pragma once

#include <algorithm> // std::clamp

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
            float penX, penY;
            auto paddleHit = GetCollisionNormal(ball.GetPosition(), ballSize, paddle.GetPosition(), paddle.GetSize(), penX, penY);
            if (paddleHit.has_value()) {
                glm::vec2 normal = paddleHit.value();
                if (normal.y > 0.0f) {
                    // Penetration resolution
                    ball.SetPosition({ ball.GetPosition().x, ball.GetPosition().y + penY });

                    // Trick ball angle
                    float paddleCenter = paddle.GetPosition().x;
                    float hitPoint = ball.GetPosition().x;

                    // Calculate how far ball hit from the center (-1 to 1) 
                    float hitFactor = (hitPoint - paddleCenter) / (paddle.GetSize().x / 2.0f);

                    // Limt to (-1 to 1) f hit the edge
                    hitFactor = std::clamp(hitFactor, -1.0f, 1.0f);

                    glm::vec2 oldVelocity = ball.GetVelocity();
                    float currentSpeed = glm::length(oldVelocity); // Save old velocity to not increment or decrement seed

                    glm::vec2 newDirection;
                    newDirection.x = hitFactor * 1.5f; // 1.5f = More angle on the edges
                    newDirection.y = 1.0f;

                    // Change angle, same speed
                    glm::vec2 newVelocity = glm::normalize(newDirection) * currentSpeed;

                    ball.Launch(newVelocity);
                }
            }

            // Bricks
            for (auto& brick : session.GetCurrentLevel().GetBricks()) {
                if (!brick.IsDestroyed()) {

                    auto hitNormal = GetCollisionNormal(ball.GetPosition(), ballSize, brick.GetPosition(), brick.GetSize(), penX, penY);

                    if (hitNormal.has_value()) {
                        glm::vec2 normal = hitNormal.value();

                        // Peneration resolution
                        // Move ball out using the normal of the colision multiply by penetration cuantity
                        ball.SetPosition({ ball.GetPosition().x + (normal.x * penX),
                            ball.GetPosition().y + (normal.y * penY) });

                        // Bounce
                        if (normal.x != 0.0f) {
                            ball.Launch({ -ball.GetVelocity().x, ball.GetVelocity().y });
                        }
                        else {
                            ball.Launch({ ball.GetVelocity().x, -ball.GetVelocity().y });
                        }

                        // Damage
                        brick.TakeDamage();
                        if (brick.IsDestroyed()) {
                            session.GetPlayer().AddScore(100);
                        }

                        break; // Collide only with one brick per frame
                    }
                }
            }

            if (ball.GetPosition().y < session.GetCurrentLevel().GetBottomLimit()) {
                session.GetPlayer().LoseLife();
                session.SetIsBallInPlay(false);
            }
        }
    }

private:
    static std::optional<glm::vec2> GetCollisionNormal(glm::vec2 posA, glm::vec2 sizeA, glm::vec2 posB, glm::vec2 sizeB, float& outPenX, float& outPenY)
    {
        float dx = posA.x - posB.x;
        float dy = posA.y - posB.y;

        float overlapX = (sizeA.x / 2.0f + sizeB.x / 2.0f) - std::abs(dx);
        float overlapY = (sizeA.y / 2.0f + sizeB.y / 2.0f) - std::abs(dy);

        if (overlapX > 0.0f && overlapY > 0.0f)
        {
            // How many pixels have overpass
            outPenX = overlapX;
            outPenY = overlapY;

            // Return the normal according to the axis of least penetration
            if (overlapX < overlapY) {
                outPenY = 0.0f; // If we crash for X, we don't correct Y
                return dx > 0.0f ? glm::vec2(1.0f, 0.0f) : glm::vec2(-1.0f, 0.0f);
            }
            else {
                outPenX = 0.0f; // If we crash for Y, we don't correct X
                return dy > 0.0f ? glm::vec2(0.0f, 1.0f) : glm::vec2(0.0f, -1.0f);
            }
        }

        return std::nullopt;
    }
};
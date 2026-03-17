#pragma once

#include <algorithm> // std::clamp

#include "Utils/Random.h"

#include "GameCore/GameSession.h"

// Data structure to return all physics events that happened during this frame
struct PhysicsReport {
    std::vector<Brick*> hitBricks;       
    std::vector<PowerUp*> hitPowerUps;   
    std::vector<Ball*> deadBalls;
    bool ballBounced = false;
};

// Handles AABB collision detection, penetration resolution, and movement
class PhysicsSystem {
public:

    PhysicsReport Update(GameSession& session, float dt) {

        PhysicsReport report;

        // --- Balls Physics ---
        for (auto& ball : session.GetBalls()) {
            ball.Move(dt);

            glm::vec2 ballSize = { ball.GetSize() * 2.0f, ball.GetSize() * 2.0f };

            auto pos = ball.GetPosition();
            float radius = ball.GetSize();

            // Screen Bounds: Left Wall
            if (pos.x - radius <= session.GetCurrentLevel().GetLeftLimit()) {
                ball.SetPosition({ session.GetCurrentLevel().GetLeftLimit() + radius, pos.y }); 
                ball.Launch({ std::abs(ball.GetVelocity().x), ball.GetVelocity().y });
                report.ballBounced = true;
            }
            // Screen Bounds: Right Wall
            else if (pos.x + radius >= session.GetCurrentLevel().GetRightLimit()) {
                ball.SetPosition({ session.GetCurrentLevel().GetRightLimit() - radius, pos.y });
                ball.Launch({ -std::abs(ball.GetVelocity().x), ball.GetVelocity().y });
                report.ballBounced = true;
            }

            // Screen Bounds: Top Ceiling
            if (pos.y + radius >= session.GetCurrentLevel().GetTopLimit()) {
                ball.SetPosition({ pos.x, session.GetCurrentLevel().GetTopLimit() - radius });
                ball.Launch({ ball.GetVelocity().x, -std::abs(ball.GetVelocity().y) });
                report.ballBounced = true;
            }

            // Paddle Collision
            auto& paddle = session.GetPaddle();
            float penX, penY;
            auto paddleHit = GetCollisionNormal(ball.GetPosition(), ballSize, paddle.GetPosition(), paddle.GetSize(), penX, penY);
            if (paddleHit.has_value()) {
                glm::vec2 normal = paddleHit.value();

                // Only bounce if the ball hits the top of the paddle
                if (normal.y > 0.0f) {
                    report.ballBounced = true;

                    // Penetration resolution: push ball out
                    ball.SetPosition({ ball.GetPosition().x, ball.GetPosition().y + penY });

                    // Dynamic bounce angle based on hit position
                    float paddleCenter = paddle.GetPosition().x;
                    float hitPoint = ball.GetPosition().x;

                    // Calculate hit factor (-1.0 to 1.0) based on distance from paddle center
                    float hitFactor = (hitPoint - paddleCenter) / (paddle.GetSize().x / 2.0f);

                    // Clamp to prevent extreme horizontal angles if hitting the exact edge
                    hitFactor = std::clamp(hitFactor, -1.0f, 1.0f);

                    glm::vec2 oldVelocity = ball.GetVelocity();
                    float currentSpeed = glm::length(oldVelocity); // Preserve current speed

                    glm::vec2 newDirection;
                    newDirection.x = hitFactor * 1.5f; // Multiplier increases the maximum exit angle
                    newDirection.y = 1.0f;

                    // Apply new direction while maintaining the exact same speed
                    glm::vec2 newVelocity = glm::normalize(newDirection) * currentSpeed;

                    ball.Launch(newVelocity);
                }
            }

            // Bricks Collision
            for (auto& brick : session.GetCurrentLevel().GetBricks()) {

                if (brick.IsDestroyed() || brick.IsDying()) {
                    continue;
                }

                auto hitNormal = GetCollisionNormal(ball.GetPosition(), ballSize, brick.GetPosition(), brick.GetSize(), penX, penY);

                if (hitNormal.has_value()) {
                    report.ballBounced = true;

                    glm::vec2 normal = hitNormal.value();

                    // Penetration resolution:
                    // Push the ball out along the collision normal multiplied by the penetration depth
                    ball.SetPosition({ ball.GetPosition().x + (normal.x * penX),
                        ball.GetPosition().y + (normal.y * penY) });

                    // Bounce reflection
                    if (normal.x != 0.0f) {
                        ball.Launch({ -ball.GetVelocity().x, ball.GetVelocity().y });
                    }
                    else {
                        ball.Launch({ ball.GetVelocity().x, -ball.GetVelocity().y });
                    }

                    // Register the hit brick
                    report.hitBricks.push_back(&brick);

                    break; // Only collide with one brick per physical frame to prevent multi-bounce glitches
                }
            }

            // Bottom Dead Zone (Lose ball)
            if (ball.GetPosition().y < session.GetCurrentLevel().GetBottomLimit()) {
                ball.Destroy();
                report.deadBalls.push_back(&ball);
            }
        }

        // --- PowerUps Physics ---
        auto& paddle = session.GetPaddle();
        for (auto& powerUp : session.GetPowerUps()) {
            if (powerUp.IsDestroyed()) continue;

            powerUp.Move(dt);

            // Destroy if it falls off screen
            if (powerUp.GetPosition().y < session.GetCurrentLevel().GetBottomLimit()) {
                powerUp.Destroy();
                continue;
            }

            // Paddle collection check (no penetration resolution needed, just overlap)
            float pX, pY;
            auto paddleHit = GetCollisionNormal(powerUp.GetPosition(), powerUp.GetSize(), paddle.GetPosition(), paddle.GetSize(), pX, pY);

            if (paddleHit.has_value()) {
                report.hitPowerUps.push_back(&powerUp);
            }
        }

        return report;
    }

private:
    // AABB Collision Check with Penetration Depth calculation
    static std::optional<glm::vec2> GetCollisionNormal(glm::vec2 posA, glm::vec2 sizeA, glm::vec2 posB, glm::vec2 sizeB, float& outPenX, float& outPenY)
    {
        float dx = posA.x - posB.x;
        float dy = posA.y - posB.y;

        float overlapX = (sizeA.x / 2.0f + sizeB.x / 2.0f) - std::abs(dx);
        float overlapY = (sizeA.y / 2.0f + sizeB.y / 2.0f) - std::abs(dy);

        // If overlapping on both axes, a collision has occurred
        if (overlapX > 0.0f && overlapY > 0.0f)
        {
            // Calculate penetration depth in pixels/units
            outPenX = overlapX;
            outPenY = overlapY;

            // Return the normal according to the axis of least penetration
            if (overlapX < overlapY) {
                outPenY = 0.0f; // If we resolve along X, ignore Y penetration
                return dx > 0.0f ? glm::vec2(1.0f, 0.0f) : glm::vec2(-1.0f, 0.0f);
            }
            else {
                outPenX = 0.0f; // If we resolve along Y, ignore X penetration
                return dy > 0.0f ? glm::vec2(0.0f, 1.0f) : glm::vec2(0.0f, -1.0f);
            }
        }

        return std::nullopt; // No collision
    }
};
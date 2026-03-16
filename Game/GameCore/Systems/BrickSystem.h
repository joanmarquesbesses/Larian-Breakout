#pragma once
#include "GameCore/GameSession.h"

class BrickSystem {
public:
	// Called when a new level is loaded to setup brick textures and animations based on their health
    void SetupBricks(GameSession& session, const std::shared_ptr<Texture2D>& spritesheet) {
        for (auto& brick : session.GetCurrentLevel().GetBricks()) {
            brick.SetMaxHealth(brick.GetHealth());

            UpdateBrickTexture(brick, spritesheet);

            int indexY = std::clamp(brick.GetMaxHealth() - 1, 0, 5);
            float pixelY = 176.0f + (indexY * 16.0f);
            float animStartPixelX = 32.0f + (5 * 32.0f);

            auto anim = Animation2D::CreateFromAtlas(spritesheet, { 32.0f, 16.0f }, { animStartPixelX, pixelY }, 5, 5, 0.1f);
            brick.SetDestroyAnimation(anim);
        }
    }

	// Called when a brick takes damage to update its texture based on remaining health
    void UpdateBrickTexture(Brick& brick, const std::shared_ptr<Texture2D>& spritesheet) {
        if (!spritesheet) return;

        int indexY = std::clamp(brick.GetMaxHealth() - 1, 0, 5);
        int damageTaken = brick.GetMaxHealth() - brick.GetHealth();
        int indexX = std::clamp(damageTaken, 0, 4);

        float pixelX = 32.0f + (indexX * 32.0f);
        float pixelY = 176.0f + (indexY * 16.0f);

        auto idleTex = SubTexture2D::CreateFromPixelCoords(spritesheet, pixelX, pixelY, 32.0f, 16.0f);
        brick.SetIdleTexture(idleTex);
    }

	// Utility function to get bricks within explosion radius of an explosive brick
    std::vector<Brick*> GetBricksInExplosion(GameSession& session, Brick* explosiveBrick) {
        std::vector<Brick*> foundBricks;

        float eHalfW = explosiveBrick->GetSize().x / 2.0f;
        float eHalfH = explosiveBrick->GetSize().y / 2.0f;

        float eMinX = explosiveBrick->GetPosition().x - (eHalfW * 1.5f);
        float eMaxX = explosiveBrick->GetPosition().x + (eHalfW * 1.5f);
        float eMinY = explosiveBrick->GetPosition().y - (eHalfH * 1.5f);
        float eMaxY = explosiveBrick->GetPosition().y + (eHalfH * 1.5f);

        for (auto& brick : session.GetCurrentLevel().GetBricks()) {
            if (&brick == explosiveBrick || brick.IsDestroyed() || brick.IsDying()) continue;

            float bHalfW = brick.GetSize().x / 2.0f;
            float bHalfH = brick.GetSize().y / 2.0f;

            float bMinX = brick.GetPosition().x - bHalfW;
            float bMaxX = brick.GetPosition().x + bHalfW;
            float bMinY = brick.GetPosition().y - bHalfH;
            float bMaxY = brick.GetPosition().y + bHalfH;

            bool collisionX = eMaxX >= bMinX && bMaxX >= eMinX;
            bool collisionY = eMaxY >= bMinY && bMaxY >= eMinY;

            if (collisionX && collisionY) {
                foundBricks.push_back(&brick);
            }
        }
        return foundBricks;
    }

    void Update(GameSession& session, float dt) {
        for (auto& brick : session.GetCurrentLevel().GetBricks()) {

            if (brick.IsDying() && !brick.IsDestroyed()) {
                brick.AddAnimTime(dt);

                auto anim = brick.GetDestroyAnim();
                if (anim) {
                    float totalAnimDuration = anim->GetFrameCount() * anim->GetFrameTime();

                    if (brick.GetAnimTime() >= totalAnimDuration) {
                        brick.SetDestroyed();
                    }
                }
                else {
                    brick.SetDestroyed();
                }
            }
        }
    }
};
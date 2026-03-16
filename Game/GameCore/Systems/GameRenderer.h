#pragma once

#include "Renderer/Renderer.h"
#include "GameCore/GameSession.h"
#include "GameCore/Systems/ParticleSystem.h"

class GameRenderer {
private:
    std::shared_ptr<Font> m_Font;
    ParticleSystem* m_ParticleSystem;

    float m_ShakeTimer = 0.0f;
    float m_ShakeIntensity = 0.05f;
    float m_ZoomTimer = 0.0f;

    std::shared_ptr<Texture2D> m_HeartTexture;
    std::shared_ptr<Texture2D> m_NebulaTexture;

    float m_NebulaOffsetV = 0.0f;
	float m_NebulaOffsetU = 0.0f;

public:
    GameRenderer(std::shared_ptr<Font> font, ParticleSystem* pSystem, 
        std::shared_ptr<Texture2D> heartTex, std::shared_ptr<Texture2D> nebulaTex)
        : m_Font(font), m_ParticleSystem(pSystem),
        m_HeartTexture(heartTex), m_NebulaTexture(nebulaTex) {
    }

    void TriggerShake(float duration, float intensity) { m_ShakeTimer = duration; m_ShakeIntensity = intensity; }
    void TriggerZoom(float duration) { m_ZoomTimer = duration; }

    void UpdateEffects(float dt) {
        if (m_ShakeTimer > 0.0f) m_ShakeTimer -= dt;
        if (m_ZoomTimer > 0.0f) m_ZoomTimer -= dt;

        m_NebulaOffsetV += dt * 0.003f;
		m_NebulaOffsetU += dt * 0.002f;
    }

    void Render(GameSession& session, bool isPaused, TextMenu* pauseMenu) {
        Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        Renderer::Clear();

        glm::mat4 viewProj = session.GetCamera().GetViewProjectionMatrix();

        glm::mat4 bgTransform = glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, 0.0f })
            * glm::scale(glm::mat4(1.0f), { 4.0f, 4.0f, 1.0f });

        glm::vec2 nebulaUVs[4] = { 
            { 0.0f + m_NebulaOffsetU, 0.0f + m_NebulaOffsetV }, 
            { 1.0f + m_NebulaOffsetU, 0.0f + m_NebulaOffsetV }, 
            { 1.0f + m_NebulaOffsetU, 1.0f + m_NebulaOffsetV }, 
            { 0.0f + m_NebulaOffsetU, 1.0f + m_NebulaOffsetV } 
        };

        if (m_ZoomTimer > 0.0f) {
            float zoomAmount = 1.0f + (m_ZoomTimer * 0.1f);
            viewProj = glm::scale(glm::mat4(1.0f), { zoomAmount, zoomAmount, 1.0f }) * viewProj;
        }

        if (m_ShakeTimer > 0.0f) {
            // Rand value between -0.5 and 0.5 and multiplied by shakeintensity
            float offsetX = ((rand() % 100) / 100.0f - 0.5f) * m_ShakeIntensity;
            float offsetY = ((rand() % 100) / 100.0f - 0.5f) * m_ShakeIntensity;
            viewProj = glm::translate(glm::mat4(1.0f), { offsetX, offsetY, 0.0f }) * viewProj;
        }

        Renderer::BeginScene(viewProj);

        if (m_NebulaTexture) {
            Renderer::DrawQuad(bgTransform, m_NebulaTexture, nebulaUVs);
        }

        auto paddleTex = session.GetPaddle().GetTexture();
        if (paddleTex) {
            Renderer::DrawQuad(session.GetPaddle().GetPosition(), session.GetPaddle().GetSize(), paddleTex);
        }
        else {
            Renderer::DrawQuad(session.GetPaddle().GetPosition(), session.GetPaddle().GetSize(), { 0.2f, 0.3f, 0.8f, 1.0f });
        }

        for (auto& ball : session.GetBalls()) {
            auto bTex = ball.GetTexture();
            if (bTex) {
                Renderer::DrawQuad(ball.GetPosition(), { ball.GetSize() * 2.0f, ball.GetSize() * 2.0f }, bTex);
            }
            else {
                Renderer::DrawQuad(ball.GetPosition(), { ball.GetSize() * 2.0f, ball.GetSize() * 2.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
            }
        }

        for (const auto& brick : session.GetCurrentLevel().GetBricks()) {
            if (brick.IsDying()) {
                auto anim = brick.GetDestroyAnim();
                if (anim) {
                    int currentFrame = (int)(brick.GetAnimTime() / anim->GetFrameTime());
                    if (currentFrame >= anim->GetFrameCount()) currentFrame = anim->GetFrameCount() - 1;

                    auto textureFrame = anim->GetFrame(currentFrame);
                    if (textureFrame) Renderer::DrawQuad(brick.GetPosition(), brick.GetSize(), textureFrame);
                }
            }
            else {
                auto idleTex = brick.GetIdleTexture();
                if (idleTex) {
                    Renderer::DrawQuad(brick.GetPosition(), brick.GetSize(), idleTex);
                }
                else {
                    Renderer::DrawQuad(brick.GetPosition(), brick.GetSize(), brick.GetColor());
                }
            }
        }

        for (const auto& powerUp : session.GetPowerUps()) {
            if (!powerUp.IsDestroyed()) { 
                auto pTex = powerUp.GetTexture();
                if (pTex) {
                    Renderer::DrawQuad(powerUp.GetPosition(), powerUp.GetSize(), pTex);
                }
                else {
                    Renderer::DrawQuad(powerUp.GetPosition(), powerUp.GetSize(), powerUp.GetColor());
                }
            }
        }

        int lives = session.GetPlayer().GetLives();

        for (int i = 0; i < lives; i++) {
            if (m_HeartTexture) {
				Renderer::DrawQuad({ 1.5f + (i * -0.15f), 0.85f }, { 0.1f, 0.08f }, m_HeartTexture);
            }
            else {
                Renderer::DrawQuad({ 1.5f + (i * -0.15f), 0.85f }, { 0.1f, 0.03f }, { 0.2f, 0.8f, 0.2f, 1.0f });
            }
        }

        std::string scoreText = "SCORE: " + std::to_string(session.GetPlayer().GetScore());
        Renderer::DrawString(scoreText, { -1.57f, 0.82f }, 0.001f, { 1.0f, 1.0f, 1.0f, 1.0f }, m_Font);

        m_ParticleSystem->OnRender();

        if (isPaused) {
            // Transparent Dark Background
            Renderer::DrawQuad({ 0.0f, 0.0f }, { 4.0f, 4.0f }, { 0.0f, 0.0f, 0.0f, 0.7f });

            // Title "PAUSED"
            float titleScale = 0.002f;
            float titleWidth = Renderer::GetTextWidth("PAUSED", titleScale, m_Font);
            Renderer::DrawString("PAUSED", { 0.0f - (titleWidth / 2.0f), 0.4f }, titleScale, { 1.0f, 1.0f, 1.0f, 1.0f }, m_Font);

            if (pauseMenu) {
                pauseMenu->OnRender();
            }
        }

        Renderer::EndScene();
    }
};
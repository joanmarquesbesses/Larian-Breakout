#pragma once

#include "Renderer/Renderer.h"
#include "GameCore/GameSession.h"
#include "GameCore/Systems/ParticleSystem.h"

struct FloatingText {
    std::string Text;
    glm::vec2 Position;
    glm::vec4 Color;
    float Timer;
    float MaxTime;
};  

class GameRenderer {
private:
    std::shared_ptr<Font> m_Font;
    ParticleSystem* m_ParticleSystem;
	ParticleSystem* m_BgParticleSystem;

    float m_ShakeTimer = 0.0f;
    float m_ShakeIntensity = 0.05f;
    float m_ZoomTimer = 0.0f;

    std::shared_ptr<Texture2D> m_HeartTexture;
    std::shared_ptr<Texture2D> m_NebulaTexture;
	std::shared_ptr<Texture2D> m_BombTexture;

    float m_NebulaOffsetV = 0.0f;
	float m_NebulaOffsetU = 0.0f;

    std::vector<FloatingText> m_FloatingTexts;

public:
	GameRenderer(std::shared_ptr<Font> font, ParticleSystem* pSystem, ParticleSystem* bgPSystem,
        std::shared_ptr<Texture2D> heartTex, std::shared_ptr<Texture2D> nebulaTex, std::shared_ptr<Texture2D> bombText)
        : m_Font(font), m_ParticleSystem(pSystem), m_BgParticleSystem(bgPSystem),
        m_HeartTexture(heartTex), m_NebulaTexture(nebulaTex), m_BombTexture(bombText) {
    }

    void TriggerShake(float duration, float intensity) { m_ShakeTimer = duration; m_ShakeIntensity = intensity; }

    void TriggerZoom(float duration) {
        m_ZoomTimer = std::max(m_ZoomTimer, duration);
    }

    void UpdateEffects(float realDt, float gameDT) {
        if (m_ShakeTimer > 0.0f) m_ShakeTimer -= realDt;
        if (m_ZoomTimer > 0.0f) m_ZoomTimer -= realDt;

        m_NebulaOffsetV += realDt * 0.006f;
		m_NebulaOffsetU += realDt * 0.005f;

        for (auto& ft : m_FloatingTexts) {
            ft.Timer -= gameDT;
            ft.Position.y += gameDT * 0.3f;
        }

        std::erase_if(m_FloatingTexts, [](const FloatingText& ft) { return ft.Timer <= 0.0f; });
    }

    void AddFloatingText(const std::string& text, glm::vec2 pos, glm::vec4 color) {
        m_FloatingTexts.push_back({ text, pos, color, 1.0f, 1.0f });
    }

    void Render(GameSession& session, bool isPaused, TextMenu* pauseMenu, float aimAngle) {
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

		m_BgParticleSystem->OnRender();

        if(aimAngle != -999.0f && !session.GetBalls().empty()) {
            std::string startText = "PRESS SPACE TO LAUNCH!";
            float textScale = 0.001f;
            float textWidth = Renderer::GetTextWidth(startText, textScale, m_Font);
            Renderer::DrawString(startText, { session.GetBalls()[0].GetPosition().x - (textWidth / 2.0f), session.GetBalls()[0].GetPosition().y + 0.1f }, textScale, { 1.0f, 1.0f, 1.0f, 1.0f }, m_Font);

            if (aimAngle != -999.0f && !session.GetBalls().empty()) {
                glm::vec2 startPos = session.GetBalls()[0].GetPosition();
                glm::vec2 dir = { std::sin(aimAngle), std::cos(aimAngle) };

                for (int i = 1; i <= 5; i++) {
                    glm::vec2 dotPos = startPos + (dir * (i * 0.15f));
                    Renderer::DrawQuad(dotPos, { 0.02f, 0.02f }, { 1.0f, 1.0f, 1.0f, 0.4f });
                }
            }
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
                    if (brick.GetMaxHealth() == -1) {
                        Renderer::DrawQuad(brick.GetPosition(), brick.GetSize(), { 0.4f, 0.4f, 0.4f, 1.0f });
                    }
                    else {
                        Renderer::DrawQuad(brick.GetPosition(), brick.GetSize(), brick.GetColor());
                    }
                }

                if (brick.IsExplosive() && m_BombTexture) {
					auto bombSize = brick.GetSize() * 0.5f;
					bombSize.x *= 0.8f; // Make it a bit narrower to fit better
                    Renderer::DrawQuad(brick.GetPosition(), bombSize, m_BombTexture);
                }
            }
        }

        for (const auto& powerUp : session.GetPowerUps()) {
            if (!powerUp.IsDestroyed()) { 
                if (powerUp.GetSubTexture()) {
                    Renderer::DrawQuad(powerUp.GetPosition(), powerUp.GetSize(), powerUp.GetSubTexture());
                }
                else if (powerUp.GetTexture()) {
                    Renderer::DrawQuad(powerUp.GetPosition(), powerUp.GetSize(), powerUp.GetTexture());
                }
                else {
                    Renderer::DrawQuad(powerUp.GetPosition(), powerUp.GetSize(), powerUp.GetColor());
                }
            }
        }

        for (const auto& ft : m_FloatingTexts) {
            float alpha = ft.Timer / ft.MaxTime;
            glm::vec4 color = ft.Color;
            color.a = alpha;

            float scale = 0.0008f;
            float width = Renderer::GetTextWidth(ft.Text, scale, m_Font);

            Renderer::DrawString(ft.Text, { ft.Position.x - (width / 2.0f), ft.Position.y }, scale, color, m_Font);
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
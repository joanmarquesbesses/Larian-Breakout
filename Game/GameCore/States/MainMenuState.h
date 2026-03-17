#pragma once
#include "IGameState.h"

#include "Renderer/OrthographicCamera.h"
#include "Renderer/Renderer.h"
#include "Assets/ResourceManager.h"
#include "Utils/Random.h"

#include "../UI/TextMenu.h"
#include "../Services/ScoreSerializer.h"

class MainMenuState : public IGameState {
private:
    OrthographicCamera m_Camera;

    std::unique_ptr<TextMenu> m_MainMenu;
    std::shared_ptr<Font> m_Font;

    GameSession* m_Session;

    float m_Time = 0.0f;
    bool m_CanContinue = false;
    bool m_IsTransitioning = false;

    std::shared_ptr<AudioClip> m_ConfirmSFX;

    std::shared_ptr<Texture2D> m_Nebula;

    float m_NebulaOffsetV = 0.0f;
    float m_NebulaOffsetU = 0.0f;
    ParticleSystem m_BgParticleSystem;

public:
    MainMenuState(GameSession* session) : m_Session(session), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) {}

    void OnEnter() override {
        std::cout << "[MainMenuState] Entering Main Menu\n";

        m_Font = ResourceManager::Get<Font>("Assets/Font/BitFont.ttf");
		m_Nebula = ResourceManager::Get<Texture2D>("Assets/Textures/nebula.png");
        AudioEngine::PlayMusic("Assets/Music/TitleScreen.mp3", true);
		m_ConfirmSFX = ResourceManager::Get<AudioClip>("Assets/SFX/Accept.mp3");

        m_Session->GetPlayer().SetHighScore(ScoreSerializer::LoadHighScore());

		m_Time = 0.0f;

        m_CanContinue = m_Session->IsGameActive();
        m_IsTransitioning = false;

        std::vector<std::string> options = { "CONTINUE", "NEW GAME", "QUIT" };
        std::vector<float> yPositions = { 0.3f, 0.0f, -0.3f }; //options y pos

        m_MainMenu = std::make_unique<TextMenu>(options, yPositions, m_Font);
        m_MainMenu->SetSelectedIndex(m_CanContinue ? 0 : 1);

        // Pre-warm background particles
        for (int i = 0; i < 40; i++) {
            ParticleProps initialStar = ParticlePresets::GetStar();
            initialStar.Position.y = Random::Range(-1.0f, 1.0f);
            initialStar.LifeTime = Random::Range(10.0f, 25.0f);
            m_BgParticleSystem.Emit(initialStar);
        }
    }

    void OnUpdate(Timestep ts) override {
        m_Time += ts.GetSeconds();

        m_NebulaOffsetV += ts * 0.003f;
        m_NebulaOffsetU += ts * 0.002f;

        if (Random::Range(0, 100) < 1) {
            m_BgParticleSystem.Emit(ParticlePresets::GetStar());
        }
        m_BgParticleSystem.OnUpdate(ts);

        // Input debouncing timeout
        if (m_Time < 0.5f) return;

        auto result = m_MainMenu->OnUpdate(m_Camera, ts.GetSeconds(), !m_CanContinue);

        // If result has a value, the user executed a menu option
        if (result.has_value()) {
            int selectedIndex = result.value();
            ExecuteAction(selectedIndex);
        }
    }

    void ExecuteAction(int index) {
        if (index == 0 && m_CanContinue && !m_IsTransitioning) { // CONTINUE
            m_IsTransitioning = true;
            if (m_RequestStateChange) {
                m_RequestStateChange(GameStateType::Playing);
                if (m_ConfirmSFX) AudioEngine::Play(m_ConfirmSFX->GetPath());
            }
        }
        else if (index == 1 && !m_IsTransitioning) { // NEW GAME
            m_Session->SetGameActive(false);
            m_IsTransitioning = true;
            if (m_RequestStateChange) {
                m_RequestStateChange(GameStateType::Playing);
                if (m_ConfirmSFX) AudioEngine::Play(m_ConfirmSFX->GetPath());
            }
        }
        else if (index == 2) { // QUIT
            std::cout << "[MainMenuState] Action: Quit Game\n";
            Application::Get().Close();
        }
    }

    void OnRender() override {
        Renderer::SetClearColor({ 0.05f, 0.05f, 0.1f, 1.0f });
        Renderer::Clear();
        Renderer::BeginScene(m_Camera.GetViewProjectionMatrix());

        if (m_Nebula) {
            glm::mat4 bgTransform = glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, 0.0f })
                * glm::scale(glm::mat4(1.0f), { 4.0f, 4.0f, 1.0f });

            glm::vec2 nebulaUVs[4] = {
                { 0.0f + m_NebulaOffsetU, 0.0f + m_NebulaOffsetV },
                { 1.0f + m_NebulaOffsetU, 0.0f + m_NebulaOffsetV },
                { 1.0f + m_NebulaOffsetU, 1.0f + m_NebulaOffsetV },
                { 0.0f + m_NebulaOffsetU, 1.0f + m_NebulaOffsetV }
            };

            Renderer::DrawQuad(bgTransform, m_Nebula, nebulaUVs, { 0.75f, 0.75f, 0.75f, 1.0f });
        }

        m_BgParticleSystem.OnRender();

        std::string scoreText = "HIGH SCORE: " + std::to_string(m_Session->GetPlayer().GetHighScore());
        Renderer::DrawString(scoreText, { -1.55f, 0.8f }, 0.001f, { 1.0f, 1.0f, 1.0f, 1.0f }, m_Font);

        // Render UI Menu
        if (m_MainMenu) {
            m_MainMenu->OnRender(!m_CanContinue);
        }

        Renderer::EndScene();
    }
};
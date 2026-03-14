#pragma once

#include <vector>
#include <string>
#include <optional>

#include "Application.h"
#include "Renderer/OrthographicCamera.h"
#include "Renderer/Renderer.h"
#include "Input/Input.h"

#include "GameCore/Controllers/PlayerController.h"

class TextMenu {
private:
    std::vector<std::string> m_Options;
    std::vector<float> m_YPositions;
    std::vector<float> m_AnimProgress;

    std::shared_ptr<Font> m_Font;

    int m_SelectedIndex = 0;
    float m_TextScale = 0.0012f;

    glm::vec4 m_DefaultColor = { 0.8f, 0.8f, 0.8f, 1.0f };
    glm::vec4 m_SelectedColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    glm::vec4 m_DisabledColor = { 0.3f, 0.3f, 0.3f, 1.0f };

public:
    TextMenu(const std::vector<std::string>& options, const std::vector<float>& yPos, std::shared_ptr<Font> font)
        : m_Options(options), m_YPositions(yPos), m_Font(font) {
        m_AnimProgress.resize(options.size(), 0.0f);
    }

    void SetSelectedIndex(int index) { m_SelectedIndex = index; }

    // Update logic and return if the selected element has been triggered
    std::optional<int> OnUpdate(const OrthographicCamera& camera, float dt, bool canJumpZero = false) {

        // Keyboard input (gamepad can be included)
        if (PlayerController::ConsumeIfPressed(PlayerAction::Down)) {
            m_SelectedIndex++;
            if (m_SelectedIndex >= m_Options.size()) m_SelectedIndex = canJumpZero ? 1 : 0;
        }
        if (PlayerController::ConsumeIfPressed(PlayerAction::Up)) {
            m_SelectedIndex--;
            int minIndex = canJumpZero ? 1 : 0;
            if (m_SelectedIndex < minIndex) m_SelectedIndex = m_Options.size() - 1;
        }

        // Mouse input
        glm::vec2 mouseWorld = GetMouseWorldPos(camera);
        float clickableHeight = 0.15f;

        for (int i = 0; i < m_Options.size(); i++) {
            if (i == 0 && canJumpZero) continue; // Jumpif continue is disabled

            float width = Renderer::GetTextWidth(m_Options[i], m_TextScale, m_Font);
            float minX = 0.0f - (width / 2.0f); // 0.0f because text is in the center of the world
            float maxX = 0.0f + (width / 2.0f);

            // Adjust box collision because text y is at the bottom
            float visualCenterY = m_YPositions[i] + 0.05f; 
            float minY = visualCenterY - (clickableHeight / 2.0f); // clickableHeight to add extra space for y collision
            float maxY = visualCenterY + (clickableHeight / 2.0f);

            // Check AABB collision
            if (mouseWorld.x >= minX && mouseWorld.x <= maxX && mouseWorld.y >= minY && mouseWorld.y <= maxY) {
                m_SelectedIndex = i; // Hover!

                if (Input::IsMouseButtonPressed(MouseButton::Left)) {
                    // TODO: Afegeix el Consume del ratolí al teu input per evitar SPAM!
                    return i;
                }
            }
        }

        // Options animation
        float animSpeed = 10.0f;
        for (int i = 0; i < m_AnimProgress.size(); i++) {
            if (i == m_SelectedIndex) {
                m_AnimProgress[i] += animSpeed * dt; // Grow
            }
            else {
                m_AnimProgress[i] -= animSpeed * dt; // Dwarf
            }
            // Fixvalue between 0.0 and 1.0
            m_AnimProgress[i] = std::clamp(m_AnimProgress[i], 0.0f, 1.0f);
        }

        // Accept input
        if (PlayerController::ConsumeIfPressed(PlayerAction::Accept)) {
            return m_SelectedIndex;
        }

        return std::nullopt; // Ningú ha acceptat res encara
    }

    void OnRender(bool isZeroDisabled = false) {
        for (int i = 0; i < m_Options.size(); i++) {
            glm::vec4 color = m_DefaultColor;

            if (i == 0 && isZeroDisabled) {
                color = m_DisabledColor;
            }
            else if (i == m_SelectedIndex) {
                color = m_SelectedColor;
            }

            // Base scale + (0 to 1 * 15% extra)
            float currentScale = m_TextScale * (1.0f + (m_AnimProgress[i] * 0.15f));

            float width = Renderer::GetTextWidth(m_Options[i], currentScale, m_Font);
            Renderer::DrawString(m_Options[i], { 0.0f - (width / 2.0f), m_YPositions[i] }, currentScale, color, m_Font);
        }
    }

private:
    glm::vec2 GetMouseWorldPos(const OrthographicCamera& camera) {
        glm::vec2 input = Input::GetMousePosition();
        auto& window = Application::Get().GetWindow();
        float width = (float)window.GetWidth();
        float height = (float)window.GetHeight();

        // Convert world coords to normalized coords (NDC: -1.0 a 1.0) Normalized Device Coordinates
        float x = (2.0f * input.x) / width - 1.0f;  // eg: (2 * 960) / 1920 - 1 = 0.0

        // Invert y value because in windows y goes down, but in game world goes up
        float y = 1.0f - (2.0f * input.y) / height;

        // Ordinary ViewProjection squashes game wolrd to be printed on the screen
        // Inverse does the oposite, takes a screen point and get the world coord of that point
        glm::mat4 invViewProj = glm::inverse(camera.GetViewProjectionMatrix());

        // Multiply our NDC point by invViewProj to obtain the real coords in the game world (eg: -1.6 a 1.6)
        glm::vec4 worldPos = invViewProj * glm::vec4(x, y, 1.0f, 1.0f); 

        return { worldPos.x, worldPos.y };
    }
};
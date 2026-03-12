#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"
#include "Shader.h"
#include "Texture2D.h"
#include "Font.h"

class Renderer
{
public:
    static void Init();
    static void Shutdown();

    // Basic commands
    static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
    static void SetClearColor(const glm::vec4& color);
    static void Clear();

    // Begin and end of draw
    static void BeginScene(const glm::mat4& viewProjectionMatrix);
    static void EndScene();

    // Draw Color
    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
    static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);

    // Draw Texture
    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
    static void DrawQuad(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
    
    // Draw Text
    static void DrawQuad(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, const glm::vec2* texCoords, const glm::vec4& tintColor = glm::vec4(1.0f));
    static void DrawString(const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color, const std::shared_ptr<Font>& font);

    // Text Util
    static float GetTextWidth(const std::string& text, float scale, const std::shared_ptr<Font>& font);

private:
    static void StartBatch();
    static void NextBatch();
    static void Flush();
};
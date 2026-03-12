#pragma once

#include <glm/glm.hpp>
#include "VertexArray.h"
#include "Shader.h"

class Renderer
{
public:
    static void Init();
    static void Shutdown();

    // Comandes bàsiques de finestra
    static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
    static void SetClearColor(const glm::vec4& color);
    static void Clear();

    // Inici i fi de la capacitat de dibuixar
    static void BeginScene(const glm::mat4& viewProjectionMatrix);
    static void EndScene();
    static void Flush(); // Envia el batch a OpenGL

    // Primitives
    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
    static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);

private:
    static void StartBatch();
    static void NextBatch();
};
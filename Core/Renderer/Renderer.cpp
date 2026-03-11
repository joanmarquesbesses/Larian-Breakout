#include "Renderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

struct QuadVertex
{
    glm::vec3 Position;
    glm::vec4 Color;
};

struct RendererData
{
    static const uint32_t MaxQuads = 1000;
    static const uint32_t MaxVertices = MaxQuads * 4;
    static const uint32_t MaxIndices = MaxQuads * 6;

    std::shared_ptr<VertexArray> QuadVertexArray;
    std::shared_ptr<VertexBuffer> QuadVertexBuffer;
    std::shared_ptr<Shader> QuadShader;

    uint32_t QuadIndexCount = 0;
    QuadVertex* QuadVertexBufferBase = nullptr;
    QuadVertex* QuadVertexBufferPtr = nullptr;

    glm::vec4 QuadVertexPositions[4];
};

static RendererData s_Data;

// Un shader super senzill integrat al codi per no dependre de fitxers .glsl
static const std::string vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

uniform mat4 u_ViewProjection;

out vec4 v_Color;

void main() {
    v_Color = a_Color;
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}
)";

static const std::string fragmentShaderSrc = R"(
#version 330 core
layout(location = 0) out vec4 color;

in vec4 v_Color;

void main() {
    color = v_Color;
}
)";

void Renderer::Init()
{
    // Opcions d'OpenGL
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    // Configurar Buffers
    s_Data.QuadVertexArray = std::make_shared<VertexArray>();
    s_Data.QuadVertexBuffer = std::make_shared<VertexBuffer>(s_Data.MaxVertices * sizeof(QuadVertex));

    BufferLayout quadLayout = {
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float4, "a_Color" }
    };
    s_Data.QuadVertexBuffer->SetLayout(quadLayout);
    s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

    s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

    // Generar Indexs per als Quads (0, 1, 2, 2, 3, 0)
    uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];
    uint32_t offset = 0;
    for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
    {
        quadIndices[i + 0] = offset + 0;
        quadIndices[i + 1] = offset + 1;
        quadIndices[i + 2] = offset + 2;
        quadIndices[i + 3] = offset + 2;
        quadIndices[i + 4] = offset + 3;
        quadIndices[i + 5] = offset + 0;
        offset += 4;
    }

    std::shared_ptr<IndexBuffer> quadIB = std::make_shared<IndexBuffer>(quadIndices, s_Data.MaxIndices);
    s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
    delete[] quadIndices;

    // Crear el Shader integrat
    s_Data.QuadShader = std::make_shared<Shader>("BasicColorShader", vertexShaderSrc, fragmentShaderSrc);

    // Posicions base d'un quadrat per a poder aplicar-hi les matrius de transformació
    s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
    s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
    s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
    s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
}

void Renderer::Shutdown()
{
    delete[] s_Data.QuadVertexBufferBase;
}

void Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    glViewport(x, y, width, height);
}

void Renderer::SetClearColor(const glm::vec4& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::BeginScene(const glm::mat4& viewProjectionMatrix)
{
    s_Data.QuadShader->Bind();
    s_Data.QuadShader->SetMat4("u_ViewProjection", viewProjectionMatrix);
    StartBatch();
}

void Renderer::EndScene()
{
    Flush();
}

void Renderer::StartBatch()
{
    s_Data.QuadIndexCount = 0;
    s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
}

void Renderer::Flush()
{
    if (s_Data.QuadIndexCount == 0) return;

    // Pugem totes les dades recollides a la gràfica de cop
    uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
    s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

    s_Data.QuadShader->Bind();
    s_Data.QuadVertexArray->Bind();
    glDrawElements(GL_TRIANGLES, s_Data.QuadIndexCount, GL_UNSIGNED_INT, nullptr);
}

void Renderer::NextBatch()
{
    Flush();
    StartBatch();
}

void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
    DrawQuad({ position.x, position.y, 0.0f }, size, color);
}

void Renderer::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
    DrawQuad(transform, color);
}

void Renderer::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
{
    if (s_Data.QuadIndexCount >= RendererData::MaxIndices) NextBatch();

    for (size_t i = 0; i < 4; i++)
    {
        s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color = color;
        ++s_Data.QuadVertexBufferPtr;
    }

    s_Data.QuadIndexCount += 6;
}
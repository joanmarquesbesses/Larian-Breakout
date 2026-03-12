#include "Renderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

struct QuadVertex
{
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 TexCoord; 
    float TexIndex;     
};

struct RendererData
{
    static const uint32_t MaxQuads = 1000;
    static const uint32_t MaxVertices = MaxQuads * 4;
    static const uint32_t MaxIndices = MaxQuads * 6;
    static const uint32_t MaxTextureSlots = 32;

    std::shared_ptr<VertexArray> QuadVertexArray;
    std::shared_ptr<VertexBuffer> QuadVertexBuffer;
    std::shared_ptr<Shader> QuadShader;
    std::shared_ptr<Texture2D> WhiteTexture;

    uint32_t QuadIndexCount = 0;
    QuadVertex* QuadVertexBufferBase = nullptr;
    QuadVertex* QuadVertexBufferPtr = nullptr;

    std::array<std::shared_ptr<Texture2D>, MaxTextureSlots> TextureSlots;
    uint32_t TextureSlotIndex = 1; // 0 = WhiteTexture

    glm::vec4 QuadVertexPositions[4];
    glm::vec2 QuadTexCoords[4];
};

static RendererData s_Data;

static const std::string vertexShaderSrc = R"(
#version 450 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;

uniform mat4 u_ViewProjection;

out vec4 v_Color;
out vec2 v_TexCoord;
out flat float v_TexIndex; // 'flat' perquè l'índex no s'interpoli entre vèrtexs

void main() {
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_TexIndex = a_TexIndex;
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}
)";

static const std::string fragmentShaderSrc = R"(
#version 450 core
layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
in flat float v_TexIndex;

uniform sampler2D u_Textures[32];

void main() {
    // Agafem el color de la textura i el multipliquem pel color base (o tint)
    vec4 texColor = texture(u_Textures[int(v_TexIndex)], v_TexCoord) * v_Color;
    if (texColor.a < 0.1) discard; // Transparència bàsica
    color = texColor;
}
)";

void Renderer::Init()
{
    // OpenGL options
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_DEPTH_TEST);

    // Buffers confgurations
    s_Data.QuadVertexArray = std::make_shared<VertexArray>();
    s_Data.QuadVertexBuffer = std::make_shared<VertexBuffer>(s_Data.MaxVertices * sizeof(QuadVertex));

    BufferLayout quadLayout = {
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float4, "a_Color" },
        { ShaderDataType::Float2, "a_TexCoord" },
        { ShaderDataType::Float,  "a_TexIndex" }
    };
    s_Data.QuadVertexBuffer->SetLayout(quadLayout);
    s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

    s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

    // Generate Indices
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

    // Generate 1x1 white texture
    TextureSpecification spec;
    spec.Width = 1;
    spec.Height = 1;
    spec.Format = ImageFormat::RGBA8;
    s_Data.WhiteTexture = std::make_shared<Texture2D>(spec);
    uint32_t whiteTextureData = 0xffffffff;
    s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

    // Initialize slots
    s_Data.TextureSlots[0] = s_Data.WhiteTexture;

    // Create Shader
    s_Data.QuadShader = std::make_shared<Shader>("BasicShader", vertexShaderSrc, fragmentShaderSrc);
    s_Data.QuadShader->Bind();

    // Tell shader which numbers has every texture slot
    int32_t samplers[s_Data.MaxTextureSlots];
    for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
        samplers[i] = i;

    s_Data.QuadShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

    s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
    s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
    s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
    s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

    s_Data.QuadTexCoords[0] = { 0.0f, 0.0f };
    s_Data.QuadTexCoords[1] = { 1.0f, 0.0f };
    s_Data.QuadTexCoords[2] = { 1.0f, 1.0f };
    s_Data.QuadTexCoords[3] = { 0.0f, 1.0f };
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
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
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
    s_Data.TextureSlotIndex = 1;
}

void Renderer::Flush()
{
    if (s_Data.QuadIndexCount == 0) return;

    uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
    s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

    for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
    {
        s_Data.TextureSlots[i]->Bind(i);
    }

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

    const float whiteTextureIndex = 0.0f;

    for (size_t i = 0; i < 4; i++)
    {
        s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->TexCoord = s_Data.QuadTexCoords[i];
        s_Data.QuadVertexBufferPtr->TexIndex = whiteTextureIndex;
        ++s_Data.QuadVertexBufferPtr;
    }
    s_Data.QuadIndexCount += 6;
}

void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tintColor)
{
    DrawQuad({ position.x, position.y, 0.0f }, size, texture, tintColor);
}

void Renderer::DrawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tintColor)
{
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
    DrawQuad(transform, texture, tintColor);
}

void Renderer::DrawQuad(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tintColor)
{
    if (s_Data.QuadIndexCount >= RendererData::MaxIndices) NextBatch();

    float textureIndex = 0.0f;
    for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
    {
        if (*s_Data.TextureSlots[i] == *texture)
        {
            textureIndex = (float)i;
            break;
        }
    }

    if (textureIndex == 0.0f)
    {
        if (s_Data.TextureSlotIndex >= RendererData::MaxTextureSlots) NextBatch();

        textureIndex = (float)s_Data.TextureSlotIndex;
        s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
        s_Data.TextureSlotIndex++;
    }

    for (size_t i = 0; i < 4; i++)
    {
        s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color = tintColor;
        s_Data.QuadVertexBufferPtr->TexCoord = s_Data.QuadTexCoords[i];
        s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        ++s_Data.QuadVertexBufferPtr;
    }
    s_Data.QuadIndexCount += 6;
}

void Renderer::DrawQuad(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, const glm::vec2* texCoords, const glm::vec4& tintColor)
{
    if (s_Data.QuadIndexCount >= RendererData::MaxIndices) NextBatch();

    float textureIndex = 0.0f;
    for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++) {
        if (*s_Data.TextureSlots[i] == *texture) {
            textureIndex = (float)i;
            break;
        }
    }

    if (textureIndex == 0.0f) {
        if (s_Data.TextureSlotIndex >= RendererData::MaxTextureSlots) NextBatch();
        textureIndex = (float)s_Data.TextureSlotIndex;
        s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
        s_Data.TextureSlotIndex++;
    }

    for (size_t i = 0; i < 4; i++) {
        s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color = tintColor;
        s_Data.QuadVertexBufferPtr->TexCoord = texCoords[i];
        s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        ++s_Data.QuadVertexBufferPtr;
    }
    s_Data.QuadIndexCount += 6;
}

void Renderer::DrawString(const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color, const std::shared_ptr<Font>& font)
{
    if (!font || !font->GetAtlas()) return;

    float xOffset = 0.0f;
    float yOffset = 0.0f;

    for (char c : text) {

        if (c >= 32 && c < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(font->GetBakedChars(), font->GetAtlas()->GetWidth(), font->GetAtlas()->GetHeight(), c - 32, &xOffset, &yOffset, &q, 1);

            float w = (q.x1 - q.x0) * scale;
            float h = (q.y1 - q.y0) * scale;

            float quadX = position.x + (q.x0 * scale) + (w / 2.0f);
            float quadY = position.y - (q.y0 * scale) - (h / 2.0f);

            glm::mat4 transform = glm::translate(glm::mat4(1.0f), { quadX, quadY, 0.0f }) * glm::scale(glm::mat4(1.0f), { w, h, 1.0f });

            glm::vec2 charTexCoords[4] = {
                { q.s0, q.t1 }, 
                { q.s1, q.t1 }, 
                { q.s1, q.t0 }, 
                { q.s0, q.t0 }  
            };

            DrawQuad(transform, font->GetAtlas(), charTexCoords, color);
        }
    }
}

float Renderer::GetTextWidth(const std::string& text, float scale, const std::shared_ptr<Font>& font)
{
    if (!font || !font->GetAtlas()) return 0.0f;
    float xOffset = 0.0f;
    float yOffset = 0.0f;

    for (char c : text) {
        if (c >= 32 && c < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(font->GetBakedChars(), font->GetAtlas()->GetWidth(), font->GetAtlas()->GetHeight(), c - 32, &xOffset, &yOffset, &q, 1);
        }
    }
    return xOffset * scale;
}

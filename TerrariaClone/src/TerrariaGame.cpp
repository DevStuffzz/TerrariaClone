#include "TerrariaGame.h"

namespace {
Mesh CreateGround()
{
    const glm::vec4 grass{0.3f, 0.7f, 0.3f, 1.0f};
    const glm::vec4 dirt{0.45f, 0.3f, 0.15f, 1.0f};
    return Mesh({
        {{-8.0f, -4.5f, 0.0f}, {0.0f, 0.0f}, dirt},
        {{ 8.0f, -4.5f, 0.0f}, {1.0f, 0.0f}, dirt},
        {{ 8.0f, -2.0f, 0.0f}, {1.0f, 1.0f}, grass},
        {{-8.0f, -2.0f, 0.0f}, {0.0f, 1.0f}, grass},
    }, {0, 1, 2, 2, 3, 0});
}
}

TerrariaGame::TerrariaGame()
    : Application({"TerrariaClone", 1280, 720, true}),
      m_camera(1.0f, 1.0f, 9.0f),
      m_ground(CreateGround())
{
    const glm::ivec2 size = GetWindow().FramebufferSize();
    OnResize(size.x, size.y);
}

void TerrariaGame::OnUpdate(float deltaTime)
{
    const Input& input = GetWindow().GetInput();
    if (input.IsKeyPressed(Key::Escape)) {
        GetWindow().Close();
    }

    glm::vec2 direction{0.0f};
    if (input.IsKeyDown(Key::A)) direction.x -= 1.0f;
    if (input.IsKeyDown(Key::D)) direction.x += 1.0f;
    if (input.IsKeyDown(Key::S)) direction.y -= 1.0f;
    if (input.IsKeyDown(Key::W)) direction.y += 1.0f;
    m_camera.Move(direction * 5.0f * deltaTime);
}

void TerrariaGame::OnRender()
{
    Renderer& renderer = GetRenderer();
    renderer.Clear({0.1f, 0.15f, 0.2f, 1.0f});
    renderer.BeginScene(m_camera);
    renderer.Draw(m_ground);
    renderer.DrawQuad({-1.0f, -2.0f}, {1.0f, 2.0f}, {0.9f, 0.8f, 0.6f, 1.0f});
}

void TerrariaGame::OnResize(int width, int height)
{
    m_camera.SetViewportSize(static_cast<float>(width), static_cast<float>(height));
}

#include "TerrariaGame.h"

namespace {
Mesh createGround()
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
      camera_(1.0f, 1.0f, 9.0f),
      ground_(createGround())
{
    const glm::ivec2 size = window().framebufferSize();
    onResize(size.x, size.y);
}

void TerrariaGame::onUpdate(float deltaTime)
{
    const Input& input = window().input();
    if (input.isKeyPressed(Key::Escape)) {
        window().close();
    }

    glm::vec2 direction{0.0f};
    if (input.isKeyDown(Key::A)) direction.x -= 1.0f;
    if (input.isKeyDown(Key::D)) direction.x += 1.0f;
    if (input.isKeyDown(Key::S)) direction.y -= 1.0f;
    if (input.isKeyDown(Key::W)) direction.y += 1.0f;
    camera_.move(direction * 5.0f * deltaTime);
}

void TerrariaGame::onRender()
{
    Renderer& renderer = this->renderer();
    renderer.clear({0.1f, 0.15f, 0.2f, 1.0f});
    renderer.beginScene(camera_);
    renderer.draw(ground_);
    renderer.drawQuad({-1.0f, -2.0f}, {1.0f, 2.0f}, {0.9f, 0.8f, 0.6f, 1.0f});
}

void TerrariaGame::onResize(int width, int height)
{
    camera_.setViewportSize(static_cast<float>(width), static_cast<float>(height));
}

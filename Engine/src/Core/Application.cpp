#include "Core/Application.h"

Application::Application(const WindowProps& props)
    : window_(props)
{
    const glm::ivec2 size = window_.framebufferSize();
    renderer_.setViewport(size.x, size.y);
    window_.setResizeCallback([this](int width, int height) {
        renderer_.setViewport(width, height);
        onResize(width, height);
    });
}

void Application::run()
{
    double lastTime = window_.time();
    while (!window_.shouldClose()) {
        const double now = window_.time();
        const float deltaTime = static_cast<float>(now - lastTime);
        lastTime = now;

        window_.pollEvents();
        onUpdate(deltaTime);
        onRender();
        window_.swapBuffers();
    }
}

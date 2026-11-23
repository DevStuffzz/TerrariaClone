#include "Core/Application.h"

Application::Application(const WindowProps& props)
    : m_window(props)
{
    const glm::ivec2 size = m_window.FramebufferSize();
    m_renderer.SetViewport(size.x, size.y);
    m_window.SetResizeCallback([this](int width, int height) {
        m_renderer.SetViewport(width, height);
        OnResize(width, height);
    });
}

void Application::Run()
{
    double lastTime = m_window.Time();
    while (!m_window.ShouldClose()) {
        const double now = m_window.Time();
        const float deltaTime = static_cast<float>(now - lastTime);
        lastTime = now;

        m_window.PollEvents();
        OnUpdate(deltaTime);
        OnRender();
        m_window.SwapBuffers();
    }
}

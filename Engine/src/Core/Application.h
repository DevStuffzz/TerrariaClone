#pragma once

#include "Core/Base.h"
#include "Core/Window.h"
#include "Renderer/Renderer.h"

// Base class for games. Derive from it, override the hooks, then create it
// from main() and call Run().
class ENGINE_API Application {
public:
    explicit Application(const WindowProps& props = {});
    virtual ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    // Runs the game loop until the window is closed.
    void Run();

protected:
    virtual void OnUpdate(float deltaTime) { (void)deltaTime; }
    virtual void OnRender() {}
    // Called after the renderer viewport has been updated to the new size.
    virtual void OnResize(int width, int height) { (void)width; (void)height; }

    Window& GetWindow() { return m_window; }
    Renderer& GetRenderer() { return m_renderer; }

private:
    // Declaration order matters: the window (and its OpenGL context) must be
    // created first and destroyed last.
    Window m_window;
    Renderer m_renderer;
};

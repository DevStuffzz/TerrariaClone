#pragma once

#include "Core/Base.h"
#include "Core/Window.h"
#include "Renderer/Renderer.h"

// Base class for games. Derive from it, override the hooks, then create it
// from main() and call run().
class ENGINE_API Application {
public:
    explicit Application(const WindowProps& props = {});
    virtual ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    // Runs the game loop until the window is closed.
    void run();

protected:
    virtual void onUpdate(float deltaTime) { (void)deltaTime; }
    virtual void onRender() {}
    // Called after the renderer viewport has been updated to the new size.
    virtual void onResize(int width, int height) { (void)width; (void)height; }

    Window& window() { return window_; }
    Renderer& renderer() { return renderer_; }

private:
    // Declaration order matters: the window (and its OpenGL context) must be
    // created first and destroyed last.
    Window window_;
    Renderer renderer_;
};

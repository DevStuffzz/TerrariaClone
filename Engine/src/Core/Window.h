#pragma once

#include "Core/Base.h"

#include "Core/Input.h"

#include <glm/vec2.hpp>

#include <functional>
#include <string>
#include <utility>

struct GLFWwindow;

struct WindowProps {
    std::string title = "TerrariaClone";
    int width = 1280;
    int height = 720;
    bool vsync = true;
};

// Owns the OS window and its OpenGL 3.3 context. Creating it initializes GLFW
// and loads GLAD, so construct it before any Renderer/Mesh/Texture/Shader and
// destroy it after them. Only one Window may exist at a time.
class ENGINE_API Window {
public:
    using ResizeCallback = std::function<void(int width, int height)>;

    explicit Window(const WindowProps& props = {});
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const;
    void close();
    // Updates input state; call once per frame before reading Input.
    void pollEvents();
    void swapBuffers();
    void setVSync(bool enabled);
    void setTitle(const std::string& title);

    glm::ivec2 framebufferSize() const;
    // Seconds since the window was created.
    double time() const;

    // Called with the new framebuffer size in pixels.
    void setResizeCallback(ResizeCallback callback) { resizeCallback_ = std::move(callback); }

    const Input& input() const { return input_; }

private:
    GLFWwindow* window_ = nullptr;
    Input input_;
    ResizeCallback resizeCallback_;
};

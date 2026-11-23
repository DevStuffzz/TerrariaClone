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

    bool ShouldClose() const;
    void Close();
    // Updates input state; call once per frame before reading Input.
    void PollEvents();
    void SwapBuffers();
    void SetVSync(bool enabled);
    void SetTitle(const std::string& title);

    glm::ivec2 FramebufferSize() const;
    // Seconds since the window was created.
    double Time() const;

    // Called with the new framebuffer size in pixels.
    void SetResizeCallback(ResizeCallback callback) { m_resizeCallback = std::move(callback); }

    const Input& GetInput() const { return m_input; }

private:
    GLFWwindow* m_window = nullptr;
    Input m_input;
    ResizeCallback m_resizeCallback;
};

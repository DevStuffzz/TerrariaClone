#include "Core/Window.h"

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

namespace {
void ErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW error " << error << ": " << description << '\n';
}

Window& Owner(GLFWwindow* window)
{
    return *static_cast<Window*>(glfwGetWindowUserPointer(window));
}
}

Window::Window(const WindowProps& props)
{
    glfwSetErrorCallback(ErrorCallback);
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_window = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    glfwMakeContextCurrent(m_window);
    if (!gladLoadGL(glfwGetProcAddress)) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        throw std::runtime_error("Failed to load OpenGL functions");
    }
    SetVSync(props.vsync);

    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
        Window& self = Owner(window);
        if (self.m_resizeCallback) {
            self.m_resizeCallback(width, height);
        }
    });
    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int, int action, int) {
        Owner(window).m_input.OnKey(key, action != GLFW_RELEASE);
    });
    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int) {
        Owner(window).m_input.OnMouseButton(button, action != GLFW_RELEASE);
    });
    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y) {
        Owner(window).m_input.OnMouseMove({static_cast<float>(x), static_cast<float>(y)});
    });
    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double x, double y) {
        Owner(window).m_input.OnScroll({static_cast<float>(x), static_cast<float>(y)});
    });
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_window) != 0;
}

void Window::Close()
{
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

void Window::PollEvents()
{
    m_input.BeginFrame();
    glfwPollEvents();
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(m_window);
}

void Window::SetVSync(bool enabled)
{
    glfwSwapInterval(enabled ? 1 : 0);
}

void Window::SetTitle(const std::string& title)
{
    glfwSetWindowTitle(m_window, title.c_str());
}

glm::ivec2 Window::FramebufferSize() const
{
    glm::ivec2 size{0};
    glfwGetFramebufferSize(m_window, &size.x, &size.y);
    return size;
}

double Window::Time() const
{
    return glfwGetTime();
}

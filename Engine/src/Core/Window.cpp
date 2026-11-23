#include "Core/Window.h"

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

namespace {
void errorCallback(int error, const char* description)
{
    std::cerr << "GLFW error " << error << ": " << description << '\n';
}

Window& owner(GLFWwindow* window)
{
    return *static_cast<Window*>(glfwGetWindowUserPointer(window));
}
}

Window::Window(const WindowProps& props)
{
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window_ = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);
    if (!window_) {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    glfwMakeContextCurrent(window_);
    if (!gladLoadGL(glfwGetProcAddress)) {
        glfwDestroyWindow(window_);
        glfwTerminate();
        throw std::runtime_error("Failed to load OpenGL functions");
    }
    setVSync(props.vsync);

    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* window, int width, int height) {
        Window& self = owner(window);
        if (self.resizeCallback_) {
            self.resizeCallback_(width, height);
        }
    });
    glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int, int action, int) {
        owner(window).input_.onKey(key, action != GLFW_RELEASE);
    });
    glfwSetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int) {
        owner(window).input_.onMouseButton(button, action != GLFW_RELEASE);
    });
    glfwSetCursorPosCallback(window_, [](GLFWwindow* window, double x, double y) {
        owner(window).input_.onMouseMove({static_cast<float>(x), static_cast<float>(y)});
    });
    glfwSetScrollCallback(window_, [](GLFWwindow* window, double x, double y) {
        owner(window).input_.onScroll({static_cast<float>(x), static_cast<float>(y)});
    });
}

Window::~Window()
{
    glfwDestroyWindow(window_);
    glfwTerminate();
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(window_) != 0;
}

void Window::close()
{
    glfwSetWindowShouldClose(window_, GLFW_TRUE);
}

void Window::pollEvents()
{
    input_.beginFrame();
    glfwPollEvents();
}

void Window::swapBuffers()
{
    glfwSwapBuffers(window_);
}

void Window::setVSync(bool enabled)
{
    glfwSwapInterval(enabled ? 1 : 0);
}

void Window::setTitle(const std::string& title)
{
    glfwSetWindowTitle(window_, title.c_str());
}

glm::ivec2 Window::framebufferSize() const
{
    glm::ivec2 size{0};
    glfwGetFramebufferSize(window_, &size.x, &size.y);
    return size;
}

double Window::time() const
{
    return glfwGetTime();
}

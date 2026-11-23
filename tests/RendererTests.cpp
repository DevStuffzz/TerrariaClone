#include "Core/Input.h"
#include "Renderer/Renderer.h"

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <cmath>
#include <cstdio>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
void glfwErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW error " << error << ": " << description << '\n';
}

void expectPixel(int x, int y, unsigned char red, unsigned char green, unsigned char blue)
{
    unsigned char pixel[4] = {};
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
    if (pixel[0] != red || pixel[1] != green || pixel[2] != blue) {
        throw std::runtime_error("Unexpected framebuffer pixel at " +
            std::to_string(x) + ", " + std::to_string(y));
    }
}
}

int main()
{
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) {
        return 1;
    }
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(64, 64, "RendererTests", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to load OpenGL functions\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    int result = 0;
    try {
        Renderer renderer;
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        renderer.setViewport(width, height);
        renderer.setViewProjection(glm::ortho(0.0f, 4.0f, 0.0f, 4.0f));
        renderer.clear({0.0f, 0.0f, 1.0f, 1.0f});
        renderer.drawQuad({1.0f, 1.0f}, {2.0f, 2.0f}, {1.0f, 0.0f, 0.0f, 1.0f});
        expectPixel(width / 2, height / 2, 255, 0, 0);
        expectPixel(width / 8, height / 8, 0, 0, 255);

        renderer.setViewProjection(glm::mat4(1.0f));
        renderer.drawQuad({-1.0f, -1.0f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f});
        expectPixel(width / 4, height / 4, 0, 255, 0);
        expectPixel(5 * width / 8, 5 * height / 8, 255, 0, 0);
        bool rejectedInvalidViewport = false;
        try {
            renderer.setViewport(-1, height);
        } catch (const std::invalid_argument&) {
            rejectedInvalidViewport = true;
        }
        if (!rejectedInvalidViewport) {
            throw std::runtime_error("Negative viewport width was accepted");
        }

        // Camera: 4 world units tall centered on (2, 2).
        Camera2D camera(static_cast<float>(width), static_cast<float>(height), 4.0f);
        camera.setPosition({2.0f, 2.0f});
        renderer.beginScene(camera);
        renderer.clear({0.0f, 0.0f, 1.0f, 1.0f});
        renderer.drawQuad({1.0f, 1.0f}, {2.0f, 2.0f}, {1.0f, 0.0f, 0.0f, 1.0f});
        expectPixel(width / 2, height / 2, 255, 0, 0);
        expectPixel(width / 8, height / 8, 0, 0, 255);
        // screenToWorld maps the top-left pixel to the view's top-left corner.
        // The visible width follows the aspect ratio, so the window need not be square.
        const glm::vec2 world = camera.screenToWorld({0.0f, 0.0f});
        const float expectedLeft = 2.0f - camera.viewSize().x * 0.5f;
        if (std::abs(world.x - expectedLeft) > 1e-4f || std::abs(world.y - 4.0f) > 1e-4f) {
            throw std::runtime_error("screenToWorld returned an unexpected position: " +
                std::to_string(world.x) + ", " + std::to_string(world.y) + " (framebuffer " +
                std::to_string(width) + "x" + std::to_string(height) + ")");
        }

        // Custom indexed mesh with per-vertex colors, positioned by a transform.
        renderer.setViewProjection(glm::ortho(0.0f, 4.0f, 0.0f, 4.0f));
        const glm::vec4 green{0.0f, 1.0f, 0.0f, 1.0f};
        const Mesh mesh({
            {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, green},
            {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, green},
            {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, green},
            {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, green},
        }, {0, 1, 2, 2, 3, 0});
        renderer.clear({0.0f, 0.0f, 1.0f, 1.0f});
        renderer.draw(mesh, Renderer::transform({0.0f, 0.0f}, {2.0f, 2.0f}));
        expectPixel(width / 4, height / 4, 0, 255, 0);
        expectPixel(3 * width / 4, 3 * height / 4, 0, 0, 255);

        bool rejectedBadIndex = false;
        try {
            Mesh invalid({Vertex{}}, {0, 1, 2});
        } catch (const std::out_of_range&) {
            rejectedBadIndex = true;
        }
        if (!rejectedBadIndex) {
            throw std::runtime_error("Mesh accepted an out-of-range index");
        }

        // 2x2 texture loaded from a PNG via stb_image: bottom row red/green,
        // top row blue/white once flipped so (0, 0) is the bottom-left.
        const unsigned char filePixels[] = {
            0, 0, 255, 255,   255, 255, 255, 255,
            255, 0, 0, 255,   0, 255, 0, 255,
        };
        const std::string texturePath = "renderer_test_texture.png";
        if (!stbi_write_png(texturePath.c_str(), 2, 2, 4, filePixels, 2 * 4)) {
            throw std::runtime_error("Failed to write test texture");
        }
        Texture texture(texturePath);
        std::remove(texturePath.c_str());
        if (texture.width() != 2 || texture.height() != 2) {
            throw std::runtime_error("Loaded texture has unexpected dimensions");
        }
        renderer.clear({0.0f, 0.0f, 0.0f, 1.0f});
        renderer.drawQuad({0.0f, 0.0f}, {4.0f, 4.0f}, texture);
        expectPixel(width / 4, height / 4, 255, 0, 0);
        expectPixel(3 * width / 4, height / 4, 0, 255, 0);
        expectPixel(width / 4, 3 * height / 4, 0, 0, 255);
        expectPixel(3 * width / 4, 3 * height / 4, 255, 255, 255);

        renderer.drawQuad({0.0f, 0.0f}, {4.0f, 4.0f}, texture, {1.0f, 0.0f, 0.0f, 1.0f});
        expectPixel(3 * width / 4, 3 * height / 4, 255, 0, 0);

        bool rejectedMissingFile = false;
        try {
            Texture missing("does_not_exist.png");
        } catch (const std::runtime_error&) {
            rejectedMissingFile = true;
        }
        if (!rejectedMissingFile) {
            throw std::runtime_error("Missing texture file was accepted");
        }

        // Input edge detection: pressed/released are true only on the frame the state changes.
        Input input;
        input.beginFrame();
        input.onKey(static_cast<int>(Key::Space), true);
        input.onKey(-1, true);
        input.onScroll({0.0f, 1.0f});
        if (!input.isKeyDown(Key::Space) || !input.isKeyPressed(Key::Space) ||
            input.scrollDelta().y != 1.0f) {
            throw std::runtime_error("Key press was not reported on its first frame");
        }
        input.beginFrame();
        if (!input.isKeyDown(Key::Space) || input.isKeyPressed(Key::Space) ||
            input.scrollDelta().y != 0.0f) {
            throw std::runtime_error("Held key or scroll carried over incorrectly");
        }
        input.onKey(static_cast<int>(Key::Space), false);
        if (input.isKeyDown(Key::Space) || !input.isKeyReleased(Key::Space)) {
            throw std::runtime_error("Key release was not reported");
        }
        std::cout << "Renderer integration test passed\n";
    } catch (const std::exception& error) {
        std::cerr << "Renderer test failed: " << error.what() << '\n';
        result = 1;
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return result;
}

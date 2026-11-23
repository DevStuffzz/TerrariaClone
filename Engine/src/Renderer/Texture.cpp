#include "Renderer/Texture.h"

#include <glad/gl.h>
#include <stb_image.h>

#include <memory>
#include <stdexcept>
#include <utility>

Texture::Texture(const std::string& path, TextureFilter filter)
{
    stbi_set_flip_vertically_on_load(1);
    int channels = 0;
    std::unique_ptr<unsigned char, decltype(&stbi_image_free)> pixels(
        stbi_load(path.c_str(), &width_, &height_, &channels, STBI_rgb_alpha),
        &stbi_image_free);
    if (!pixels) {
        throw std::runtime_error("Failed to load texture '" + path + "': " +
                                 stbi_failure_reason());
    }
    create(pixels.get(), filter);
}

Texture::Texture(int width, int height, const unsigned char* rgbaPixels, TextureFilter filter)
    : width_(width), height_(height)
{
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Texture dimensions must be positive");
    }
    if (!rgbaPixels) {
        throw std::invalid_argument("Texture pixel data cannot be null");
    }
    create(rgbaPixels, filter);
}

void Texture::create(const unsigned char* rgbaPixels, TextureFilter filter)
{
    glGenTextures(1, &texture_);
    if (!texture_) {
        throw std::runtime_error("Failed to create OpenGL texture");
    }

    const int glFilter = filter == TextureFilter::Nearest ? GL_NEAREST : GL_LINEAR;
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, rgbaPixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
    glDeleteTextures(1, &texture_);
}

Texture::Texture(Texture&& other) noexcept
    : texture_(std::exchange(other.texture_, 0)),
      width_(std::exchange(other.width_, 0)),
      height_(std::exchange(other.height_, 0))
{
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this != &other) {
        glDeleteTextures(1, &texture_);
        texture_ = std::exchange(other.texture_, 0);
        width_ = std::exchange(other.width_, 0);
        height_ = std::exchange(other.height_, 0);
    }
    return *this;
}

void Texture::bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture_);
}

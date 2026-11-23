#pragma once

#include "Core/Base.h"

#include <string>

enum class TextureFilter {
    Nearest,
    Linear
};

class ENGINE_API Texture {
public:
    // Loads PNG/JPG/BMP/TGA/etc. via stb_image. Images are flipped vertically
    // so texture coordinate (0, 0) is the bottom-left of the image.
    explicit Texture(const std::string& path, TextureFilter filter = TextureFilter::Nearest);
    // Creates a texture from tightly packed RGBA8 pixels, bottom row first.
    Texture(int width, int height, const unsigned char* rgbaPixels,
            TextureFilter filter = TextureFilter::Nearest);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    void bind(unsigned int slot = 0) const;

    int width() const { return width_; }
    int height() const { return height_; }
    unsigned int id() const { return texture_; }

private:
    void create(const unsigned char* rgbaPixels, TextureFilter filter);

    unsigned int texture_ = 0;
    int width_ = 0;
    int height_ = 0;
};

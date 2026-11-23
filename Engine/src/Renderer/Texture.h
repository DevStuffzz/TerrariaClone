#pragma once

#include "Core/Base.h"

#include <string>

enum class TextureFilter {
    Nearest,
    Linear
};

struct TextureCoords {
    float u0, v0;
    float u1, v1;
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

    void Bind(unsigned int slot = 0) const;

    int Width() const { return m_width; }
    int Height() const { return m_height; }
    unsigned int Id() const { return m_texture; }

private:
    void Create(const unsigned char* rgbaPixels, TextureFilter filter);

    unsigned int m_texture = 0;
    int m_width = 0;
    int m_height = 0;
};


class ENGINE_API TextureAtlas : public Texture {
public:
    TextureAtlas(const std::string& path,
        int tileWidth, int tileHeight,
        TextureFilter filter = TextureFilter::Nearest);

    TextureCoords GetCoords(int column, int row) const; // Get the texture coordinates for a specific tile in the atlas
    TextureCoords GetCoords(int id) const;              // Automatically get the column and row from the id

    int Columns() const { return m_columns; }
    int Rows() const { return m_rows; }

private:
    int m_columns = 0;
    int m_rows = 0;
    int m_tileWidth = 0;
    int m_tileHeight = 0;
};
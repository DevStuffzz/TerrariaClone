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
        stbi_load(path.c_str(), &m_width, &m_height, &channels, STBI_rgb_alpha),
        &stbi_image_free);
    if (!pixels) {
        throw std::runtime_error("Failed to load texture '" + path + "': " +
                                 stbi_failure_reason());
    }
    Create(pixels.get(), filter);
}

Texture::Texture(int width, int height, const unsigned char* rgbaPixels, TextureFilter filter)
    : m_width(width), m_height(height)
{
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Texture dimensions must be positive");
    }
    if (!rgbaPixels) {
        throw std::invalid_argument("Texture pixel data cannot be null");
    }
    Create(rgbaPixels, filter);
}

void Texture::Create(const unsigned char* rgbaPixels, TextureFilter filter)
{
    glGenTextures(1, &m_texture);
    if (!m_texture) {
        throw std::runtime_error("Failed to create OpenGL texture");
    }

    const int glFilter = filter == TextureFilter::Nearest ? GL_NEAREST : GL_LINEAR;
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, rgbaPixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_texture);
}

Texture::Texture(Texture&& other) noexcept
    : m_texture(std::exchange(other.m_texture, 0)),
      m_width(std::exchange(other.m_width, 0)),
      m_height(std::exchange(other.m_height, 0))
{
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this != &other) {
        glDeleteTextures(1, &m_texture);
        m_texture = std::exchange(other.m_texture, 0);
        m_width = std::exchange(other.m_width, 0);
        m_height = std::exchange(other.m_height, 0);
    }
    return *this;
}

void Texture::Bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}



// -----------------------------
// TextureAtlas implementation
// -----------------------------

TextureAtlas::TextureAtlas(const std::string& path,
    int tileWidth, int tileHeight,
    TextureFilter filter)
    : Texture(path, filter),
    m_tileWidth(tileWidth),
    m_tileHeight(tileHeight)
{
    m_columns = Width() / m_tileWidth;
    m_rows = Height() / m_tileHeight;
}

TextureCoords TextureAtlas::GetCoords(int column, int row) const
{
    float u0 = static_cast<float>(column * m_tileWidth) / Width();
    float v0 = static_cast<float>(row * m_tileHeight) / Height();

    float u1 = static_cast<float>((column + 1) * m_tileWidth) / Width();
    float v1 = static_cast<float>((row + 1) * m_tileHeight) / Height();

    return { u0, v0, u1, v1 };
}

TextureCoords TextureAtlas::GetCoords(int id) const
{
	int column = id % m_columns;
	int row = id / m_columns;
	return GetCoords(column, row);
}
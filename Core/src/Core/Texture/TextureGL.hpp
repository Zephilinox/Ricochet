#pragma once

//SELF

//LIBS

//STD
#include <array>
#include <vector>
#include <functional>

namespace core
{

struct Pixel
{
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
    std::uint8_t a = 0;
};

class TextureGL
{
public:
    TextureGL();
    void load(std::vector<Pixel> pixels, unsigned int width, unsigned int height);
    const std::vector<Pixel>& getPixels() const;
    const Pixel& operator[](unsigned int index) const;

    void update(std::function<void(std::vector<Pixel>& pixels)> updater);
    unsigned int getOpenglTextureID() const;

    [[nodiscard]] unsigned int getWidth() const;
    [[nodiscard]] unsigned int getHeight() const;

private:
    unsigned int width = 0;
    unsigned int height = 0;
    std::vector<Pixel> pixels;

    unsigned int opengl_texture_id = 0;
};

} // namespace core
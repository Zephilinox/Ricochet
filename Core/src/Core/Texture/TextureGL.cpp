#include "TextureGL.hpp"

//SELF

//LIBS
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>

    #define GL_GLEXT_PROTOTYPES 1
    #include <SDL_opengles2.h>
#else
    #undef main
    #include <glad/glad.h>
#endif

#include <spdlog/spdlog.h>

//STD

using namespace core;

inline void magic_function()
{
    auto err = glGetError();
    if (err != GL_NO_ERROR)
    {
        //spdlog::info("OpenGL error - {:x}", err);
        int totes_important = 0;
    }
}

TextureGL::TextureGL()
{
    magic_function();
    glGenTextures(1, &opengl_texture_id);
    magic_function();
}

void TextureGL::load(std::vector<Pixel> p, unsigned int w, unsigned int h)
{
    pixels = std::move(p);
    pixels.resize(w * h);
    width = w;
    height = h;

    magic_function();
    glBindTexture(GL_TEXTURE_2D, opengl_texture_id);
    magic_function();
    // set the texture wrapping parameters
    magic_function();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
    magic_function();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    magic_function();
    // set texture filtering parameters
    magic_function();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    magic_function();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    magic_function();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    magic_function();
    glGenerateMipmap(GL_TEXTURE_2D);
    magic_function();
}

const Pixel& TextureGL::operator[](unsigned int index) const
{
    return pixels[index];
}

void TextureGL::update(std::function<void(std::vector<Pixel>& pixels)> updater)
{
    magic_function();
    updater(pixels);
    magic_function();

    magic_function();
    glBindTexture(GL_TEXTURE_2D, opengl_texture_id);
    magic_function();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    magic_function();
    glGenerateMipmap(GL_TEXTURE_2D);
    magic_function();
}

unsigned int TextureGL::getOpenglTextureID() const
{
    return opengl_texture_id;
}

unsigned int TextureGL::getWidth() const
{
    return width;
}

unsigned int TextureGL::getHeight() const
{
    return height;
}

const std::vector<Pixel>& TextureGL::getPixels() const
{
    return pixels;
}
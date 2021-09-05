#pragma once

//SELF
#include "Core/Texture/TextureGL.hpp"

//LIBS
#include <glm/glm.hpp>

//STD
#include <algorithm>
#include <cmath>

namespace core
{

struct Frame
{
    Frame(unsigned int width, unsigned int height)
    {
        pixels_width = width;
        pixels_height = height;
        pixels.resize(width * height);
    }

    glm::vec3 pixel(unsigned int width, unsigned int height){
        return pixels[pixels_width * width + height];
    }

    std::vector<std::uint8_t> toRGB24()
    {
        std::vector<std::uint8_t> rgb_pixels;
        rgb_pixels.resize(pixels.size() * 3);

        for (std::size_t i = 0; i < rgb_pixels.size(); i += 3)
        {
            //sqrt for gamma 2
            glm::vec3 rgb_range = glm::sqrt(pixels[i / 3]) * 255.0f; // 0.0f = 0, 1.0f = 255
            //rgb_range.x = std::clamp(rgb_range.x, 0.0f, 255.0f);
            //rgb_range.y = std::clamp(rgb_range.y, 0.0f, 255.0f);
            //rgb_range.z = std::clamp(rgb_range.z, 0.0f, 255.0f);

            rgb_pixels[i] = static_cast<std::uint8_t>(rgb_range.x);
            rgb_pixels[i + 1] = static_cast<std::uint8_t>(rgb_range.y);
            rgb_pixels[i + 2] = static_cast<std::uint8_t>(rgb_range.z);

            if (rgb_pixels[i] == 0 && rgb_pixels[i+1] == 0 && rgb_pixels[i+2] == 0)
                int iadfjhiafasf = 0;
        }

        return rgb_pixels;
    }

    void toPixels(std::vector<core::Pixel>& rgba_pixels)
    {
        rgba_pixels.resize(pixels.size());

        for (std::size_t i = 0; i < rgba_pixels.size(); ++i)
        {
            //sqrt for gamma 2
            rgba_pixels[i] = core::Pixel{
                static_cast<std::uint8_t>(std::sqrt(pixels[i].x) * 255.999f),
                static_cast<std::uint8_t>(std::sqrt(pixels[i].y) * 255.999f),
                static_cast<std::uint8_t>(std::sqrt(pixels[i].z) * 255.999f),
                255
            };
        }
    }

    std::string toPPM()
    {
        std::string contents;

        auto rgb = toRGB24();
        contents += "P3\n";
        contents += std::to_string(pixels_width) + " " + std::to_string(pixels_height) + "\n";
        contents += "255\n";

        for (std::size_t i = 0; i < rgb.size(); i += 3)
        {
            contents += std::to_string(rgb[i]) + " ";
            contents += std::to_string(rgb[i + 1]) + " ";
            contents += std::to_string(rgb[i + 2]) + "\n";
        }

        return contents;
    }

    unsigned int pixels_width;
    unsigned int pixels_height;
    std::vector<glm::vec3> pixels;
};

} // namespace core
#pragma once

//SELF

//LIBS
#include <glm/glm.hpp>

//STD

namespace core
{

struct Camera
{
    struct Settings
    {
        unsigned int width = 800;
        unsigned int height = 450;
        double viewport_height = 2.0;
        double focal_length = 1.0;
        glm::highp_dvec3 origin{0.0, 0.0, 0.0};
    };

    Camera()
        : Camera(Settings())
    {
        
    }

    Camera(Settings settings)
        : aspect_ratio(static_cast<double>(settings.width) / static_cast<double>(settings.height))
        , width(settings.width)
        , height(settings.height)
        , viewport_height(settings.viewport_height)
        , viewport_width(viewport_height * aspect_ratio)
        , horizontal{viewport_width, 0.0, 0.0}
        , vertical{0.0, viewport_height, 0.0}
        , origin(settings.origin)
        , focal_length(settings.focal_length)
        , lower_left_corner(origin - horizontal/2.0 - vertical/2.0 - glm::highp_dvec3(0.0, 0.0, focal_length))
    {

    }
    
    double aspect_ratio;
    unsigned int width;
    unsigned int height;
    double viewport_height;
    double viewport_width;
    glm::highp_dvec3 horizontal;
    glm::highp_dvec3 vertical;
    glm::highp_dvec3 origin;
    double focal_length;
    glm::highp_dvec3 lower_left_corner;
};

} // namespace core
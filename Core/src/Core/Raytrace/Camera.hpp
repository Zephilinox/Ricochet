#pragma once

//SELF

//LIBS
#include <glm/glm.hpp>

//STD

namespace core
{

struct Camera
{
    //const float upscale = 1.0f;
    const double aspect_ratio = 16.0 / 9.0;
    const unsigned int width = 800;
    const unsigned int height = 800 / aspect_ratio;
    //float zoom = 0.5f;
    //const float field_of_view = static_cast<float>(width) / 1280.0 * zoom;
    //const float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    const double viewport_height = 2.0;
    const double viewport_width = aspect_ratio * viewport_height;
   // glm::vec3 horizontal{ 4.0f / upscale * aspect_ratio * field_of_view, 0.0f, 0.0f };
    //glm::vec3 vertical{ 0.0f, 4.0f / upscale * field_of_view, 0.0f };
    glm::highp_dvec3 horizontal{ viewport_width, 0.0, 0.0 };
    glm::highp_dvec3 vertical{ 0.0, viewport_height, 0.0 };
    glm::highp_dvec3 origin{0.0, 0.0, 0.0};
    const double focal_length = 1.0;
    glm::highp_dvec3 lower_left_corner = origin - horizontal/2.0 - vertical/2.0 - glm::highp_dvec3(0.0, 0.0, focal_length);
    //glm::vec3 lower_left_corner{ -5.0f * zoom, -2.0f * zoom, -10.0f };
};

} // namespace core
#pragma once

//SELF

//LIBS
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

//STD
#include <cmath>

namespace core
{

inline glm::highp_dvec3 random_position_on_unit_sphere()
{
    /*auto magic = [](glm::vec2 O) {
        const double a = 2.0 * O.x - 1.0;
        const double b = 2.0 * O.y - 1.0;
        if (a * a > b * b) // use squares instead of absolute values
        { 
            float r = a;
            float phi = (3.141592653 / 4.0) * (b / a);
            return glm::vec2(r * std::cos(phi), r * std::sin(phi));
        }

        float r = b;
        float phi = (3.141592653 / 4.0) * (a / b) + (3.141592653 / 2.0);
        return glm::vec2(r * std::cos(phi), r * std::sin(phi));
    };

    glm::highp_dvec3 position{ 0.0, 0.0, 0.0 };
    auto magic_vec2 = magic(glm::vec2{
        static_cast<double>(std::rand() % 100000) / 100000.0f,
        static_cast<double>(std::rand() % 100000) / 100000.0f
    });

    position = glm::vec3{
        magic_vec2.x,
        magic_vec2.y,
        1.0 - std::sqrt((magic_vec2.x * magic_vec2.x) + (magic_vec2.y * magic_vec2.y))
    };
    return position;*/
    glm::highp_dvec3 position{ 0.0, 0.0, 0.0 };

    while (true)
    {
        //todo
        auto rand_double = []() {
            return (static_cast<double>(std::rand() % 20000) / 10000.0f) - 1.0f;
        };

        position = glm::highp_dvec3{ rand_double(), rand_double(), rand_double() };
        if (glm::length2(position) < 1.0)
            break;
    }

    return glm::normalize(position);
}

} // namespace core
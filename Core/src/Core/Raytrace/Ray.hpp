#pragma once

//SELF

//LIBS
#include <glm/glm.hpp>

//STD

namespace core
{

struct Ray
{
    glm::highp_dvec3 origin;
    glm::highp_dvec3 direction;

    glm::highp_dvec3 point_at_time(double time) const
    {
        return origin + (direction * time);
    }
};

} // namespace core
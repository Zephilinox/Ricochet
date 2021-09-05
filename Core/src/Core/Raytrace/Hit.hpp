#pragma once

//SELF
#include "Ray.hpp"

//LIBS
#include <glm/glm.hpp>

//STD
#include <optional>

namespace core
{

struct Material;

struct HitInfo
{
    double time = 0;
    glm::highp_dvec3 position{};
    glm::highp_dvec3 normal{};
    glm::highp_dvec3 colour{};
    Material* material = nullptr;
};

struct Hitable
{
    virtual std::optional<HitInfo> hit(const Ray& ray, float time_min, float time_max) const = 0;
};

} // namespace core
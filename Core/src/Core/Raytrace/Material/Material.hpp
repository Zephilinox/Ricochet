#pragma once

//SELF
#include "Core/Raytrace/Utility.hpp"
#include "Core/Raytrace/Ray.hpp"

//LIBS
#include <glm/glm.hpp>

//STD
#include <variant>
#include <string>
#include <optional>

namespace core
{

struct HitInfo;

struct BounceInfo
{
    Ray ray;
    glm::highp_dvec3 attenuation{};
    double absorption = 0;
    double probability_density = 0;
};

struct Material
{
    virtual std::optional<BounceInfo> bounce(const Ray& ray, float time_min, float time_max, const HitInfo& hit_info) const = 0;
};

struct Diffuse final : public Material
{
    std::optional<BounceInfo> bounce(const Ray& ray, float time_min, float time_max, const HitInfo& hit_info) const final;

    glm::highp_dvec3 albedo{};
};

struct Metal final : public Material
{
    std::optional<BounceInfo> bounce(const Ray& ray, float time_min, float time_max, const HitInfo& hit_info) const final;

    double fuzz = 0.0f;
};

} // namespace core
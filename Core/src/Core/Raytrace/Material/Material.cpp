#include "Material.hpp"

//SELF
#include "Core/Raytrace/Hit.hpp"

//LIBS

//STD

using namespace core;

inline glm::highp_dvec3 reflect(glm::highp_dvec3 a, glm::highp_dvec3 b)
{
    return a - 2.0 * glm::dot(a, b) * b;
}

std::optional<BounceInfo> Diffuse::bounce(const Ray& ray, float time_min, float time_max, const HitInfo& hit_info) const
{
    auto rand_normal = hit_info.normal + random_position_on_unit_sphere();
    rand_normal = glm::normalize(rand_normal);
    const auto target = hit_info.position + rand_normal;

    const auto intersect_bias = rand_normal * 0.0001;
    const auto direction = glm::normalize(target - hit_info.position - intersect_bias);
    auto out_ray = Ray{ hit_info.position + intersect_bias, direction };
    float probability_density = glm::dot(out_ray.direction, hit_info.normal) / 3.1415192653;

    return BounceInfo{
        out_ray,
        hit_info.colour * glm::dot(out_ray.direction, hit_info.normal) * glm::highp_dvec3(1.0),
        0.9f,
        probability_density //todo: do something with this
    };
}

std::optional<BounceInfo> Metal::bounce(const Ray& ray, float time_min, float time_max, const HitInfo& hit_info) const
{
    auto reflected = reflect(glm::normalize(ray.direction), hit_info.normal + fuzz * random_position_on_unit_sphere());
    reflected = glm::normalize(reflected);
    const auto out_ray = Ray{ hit_info.position, reflected };
    return BounceInfo{
        out_ray,
        hit_info.colour,
        0.8f
    };
}

#include "Shape.hpp"

using namespace core;

std::optional<HitInfo> Cube::hit(const Ray& ray, float time_min, float time_max) const
{
    return std::nullopt;
}

std::optional<HitInfo> Sphere::hit(const Ray& ray, float time_min, float time_max) const
{
    const auto oc = ray.origin - center;
    const auto a = glm::dot(ray.direction, ray.direction);
    const auto b = glm::dot(oc, ray.direction);
    const auto c = glm::dot(oc, oc) - (radius * radius);
    const auto discriminant = (b * b) - (a * c);

    if (discriminant <= 0.0)
        return std::nullopt;

    auto hit_info = HitInfo{
        0.0f,
        glm::highp_dvec3{ 0.0, 0.0, 0.0 },
        glm::highp_dvec3{ 0.0, 0.0, 0.0 },
        colour,
        material
    };

    auto temp = (-b - std::sqrt(discriminant)) / a;
    if (temp < time_max && temp > time_min)
    {
        hit_info.time = temp;
        hit_info.position = ray.point_at_time(temp);
        hit_info.normal = (hit_info.position - center) / radius;
        return hit_info;
    }

    temp = (-b + std::sqrt(discriminant)) / a;
    if (temp < time_max && temp > time_min)
    {
        hit_info.time = temp;
        hit_info.position = ray.point_at_time(temp);
        hit_info.normal = (hit_info.position - center) / radius;
        return hit_info;
    }

    return std::nullopt;
}

std::optional<HitInfo> Triangle::hit(const Ray& ray, float time_min, float time_max) const
{
    return std::nullopt;
}

std::optional<HitInfo> Model::hit(const Ray& ray, float time_min, float time_max) const
{
    return std::nullopt;
}
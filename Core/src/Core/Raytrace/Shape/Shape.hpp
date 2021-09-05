#pragma once

//SELF
#include "Core/Raytrace/Hit.hpp"

//LIBS
#include <glm/glm.hpp>

//STD
#include <variant>
#include <string>
#include <vector>
#include <optional>
#include <array>

namespace core
{

struct Material;

struct Cube : public Hitable
{
    std::optional<HitInfo> hit(const Ray& ray, float time_min, float time_max) const final;

    glm::highp_dvec3 center{};
    float extents = 0.5;
    glm::highp_dvec3 colour{ 1.0f, 0.0f, 1.0f };
    Material* material = nullptr;
};

struct Sphere : public Hitable
{
    std::optional<HitInfo> hit(const Ray& ray, float time_min, float time_max) const final;

    glm::highp_dvec3 center{};
    double radius = 0;
    glm::highp_dvec3 colour{ 1.0f, 1.0f, 0.0f };
    Material* material = nullptr;
};

struct Triangle : public Hitable
{
    std::optional<HitInfo> hit(const Ray& ray, float time_min, float time_max) const final;

    std::array<glm::highp_dvec3, 3> vertices{};
    glm::highp_dvec3 normal{ 0.5f, 0.5f, 0.5f };
    glm::highp_dvec3 colour{ 0.0f, 0.0f, 1.0f };
    Material* material;
};

struct Model final : public Hitable
{
    std::optional<HitInfo> hit(const Ray& ray, float time_min, float time_max) const final;

    std::vector<Triangle> triangles;
};

using Shape = std::variant<Cube, Sphere, Triangle, Model>;

} // namespace core
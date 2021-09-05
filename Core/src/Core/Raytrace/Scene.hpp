#pragma once

//SELF
#include "Hit.hpp"

//LIBS

//STD
#include <vector>
#include <memory>

namespace core
{

struct Scene : public Hitable
{
    
    std::optional<HitInfo> hit(const Ray& ray, float time_min, float time_max) const final
    {
		float closest = time_max;
		std::optional<HitInfo> temp_info;

		for (auto& hitable : hitables)
		{
			auto maybe_hit = hitable->hit(ray, time_min, closest);
			if (maybe_hit && maybe_hit->time < closest)
			{
				closest = maybe_hit->time;
				temp_info = maybe_hit;
			}
		}

		return temp_info;
	}
    std::vector<std::unique_ptr<Hitable>> hitables;
};

} // namespace core
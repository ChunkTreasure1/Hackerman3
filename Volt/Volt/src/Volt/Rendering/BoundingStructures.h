#pragma once

#include <GEM/gem.h>

namespace Volt
{
	struct BoundingSphere
	{
		gem::vec3 center = 0.f;
		float radius = 0.f;
	};
}
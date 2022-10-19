#pragma once
#include "GEM/vector/vector.h"

namespace Volt{
	struct Particle{
		gem::vec4 startColor;
		gem::vec4 endColor;
		gem::vec4 color;

		gem::vec3 direction;
		gem::vec3 startPosition;
		gem::vec3 endPosition;
		gem::vec3 position;

		gem::vec3 gravity;

		float velocity;
		float startVelocity;
		float endVelocity;

		float emissiveness;
		bool fade;

		float distance;
		float endDistance;

		bool dead = true;
	};
}
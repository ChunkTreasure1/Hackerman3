#pragma once
#include "Plane.hpp"
#include "Ray.hpp"

#include <gem/gem.h>
#include <math.h>

namespace Volt
{
	// If the ray is parallel to the plane, aOutIntersectionPoint remains unchanged. If
	// the ray is in the plane, true is returned, if not, false is returned. If the ray
	// isn't parallel to the plane, the intersection point is stored in
	// aOutIntersectionPoint and true returned.
	template<typename T>
	bool IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay, gem::vec<3, T>&
		aOutIntersectionPoint);
}

template<typename T>
bool Volt::IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay, gem::vec<3, T>& aOutIntersectionPoint)
{
	Plane<T> plane = aPlane;
	Ray<T> ray = aRay;

	T denom = plane.GetNormal().Dot(ray.GetDirection());
	gem::vec<3, T> distance = plane.GetPosition() - ray.GetOrigin();
	T pointLength;

	if (denom == 0)
	{
		return (distance.Dot(plane.GetNormal()) == 0);
	}
	else if (std::abs(denom) > std::numeric_limits<T>::epsilon())
	{
		pointLength = distance.Dot(plane.GetNormal()) / denom;

		aOutIntersectionPoint = ray.GetOrigin() + ray.GetDirection().GetNormalized() * pointLength;

		return (pointLength >= 0);
	}

	return false;
}
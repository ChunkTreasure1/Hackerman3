#pragma once
#include "Volt/Core/Base.h"
#include "Volt/AI/NavMesh/NavMeshData.h"

#include <gem/gem.h>
#include <vector>
#include <algorithm>
#include <functional>

namespace Volt
{
	struct Point
	{
		Point() {}
		Point(const MeshIndex& ind, const gem::vec3& pos)
			: position(pos), index(ind) {}

		Point(const MeshIndex& ind, const float& x, const float& y, const float& z)
			: position(gem::vec3(x, y, z)), index(ind) {}

		MeshIndex index = 0;
		gem::vec3 position = { 0.f, 0.f, 0.f };

		Ref<Point> next;
		Ref<Point> previous;
	};

	struct Polygon
	{
		Polygon ClonePolygon() const
		{
			// Create clone with new ptrs
			Polygon clone;

			for (auto& p : points)
			{
				clone.points.emplace_back(CreateRef<Point>(p->index, p->position));
			}

			for (auto& p : clone.points)
			{
				const auto& orgP = GetPointFromIndex(p->index);
				p->next = clone.GetPointFromIndex(orgP->next->index);
				p->previous = clone.GetPointFromIndex(orgP->previous->index);
			}

			return clone;
		}

		Ref<Point> GetPointFromIndex(const MeshIndex& index) const
		{
			for (uint32_t i = 0; i < points.size(); i++) 
			{
				if (points[i]->index == index)
				{
					return points[i];
				}
			}
			return Ref<Point>();
		}

		void RemovePointWithIndex(const MeshIndex& index)
		{
			Ref<Point> p = GetPointFromIndex(index);

			if (p)
			{
				p->next->previous = p->previous;
				p->previous->next = p->next;

				points.erase(std::find(points.begin(), points.end(), p));
			}
		}

		void ForEach(std::function<void(Ref<Point>)> fnc)
		{
			if (!points.empty())
			{
				Ref<Point> currentPoint = points[0];
				for (uint32_t i = 0; i < points.size(); i++)
				{
					if (currentPoint)
					{
						fnc(currentPoint);
						currentPoint = currentPoint->next;
					}
				}
			}
		}

		uint32_t GetVertexCount() const { return (uint32_t)points.size(); }

		std::vector<Ref<Point>> points;
	};
}
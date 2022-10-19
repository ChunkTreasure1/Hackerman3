#pragma once
#include "Volt/AI/NavMesh/NavMeshData.h"
#include "Volt/AI/NavMesh/Triangulation/TriangulationStructs.h"

#include <vector>

namespace TriangulationUtils
{
	bool contains(const std::vector<uint32_t>& vec, const uint32_t& elem);
}

namespace Volt
{
	Polygon CreatePolygon(const MeshInfo& meshInfo);
	Polygon MergePolygons(const Polygon& a, const Polygon& b);
	Polygon MergePolygons(const std::vector<Polygon>& polygons);
	std::vector<uint32_t> TriangulatePolygon(const Polygon& polygon);
}
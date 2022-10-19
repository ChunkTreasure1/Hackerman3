#include "vtpch.h"
#include "Triangulation.h"
#include "Volt/Log/Log.h"
#include "Volt/AI/NavMesh/Math/LineVolume.hpp"

namespace TriangulationUtils
{
	bool contains(const std::vector<uint32_t>& vec, const uint32_t& elem)
	{
		bool result = false;
		if (std::find(vec.begin(), vec.end(), elem) != vec.end())
		{
			result = true;
		}
		return result;
	}
}

namespace Volt
{
	bool IsInTriangle(Ref<Point> point, Ref<Point> v1, Ref<Point> v2, Ref<Point> v3)
	{
		gem::vec3 p = point->position;
		gem::vec3 a = v1->position;
		gem::vec3 b = v2->position;
		gem::vec3 c = v3->position;

		a -= p;
		b -= p;
		c -= p;

		p -= p;

		gem::vec3 u = gem::cross(b, c);
		gem::vec3 v = gem::cross(c, a);
		gem::vec3 w = gem::cross(a, b);

		if (gem::dot(u, v) < 0.f) 
		{
			return false;
		}
		if (gem::dot(u, w) < 0.0f)
		{
			return false;
		}

		return true;
	}

	std::vector<Ref<Point>> GetPointsInTriangle(Polygon polygon, Ref<Point> v1, Ref<Point> v2, Ref<Point> v3)
	{
		std::vector<Ref<Point>> result;

		for (const auto& point : polygon.points)
		{
			if (point->index == v1->index || point->index == v2->index || point->index == v3->index) { continue; }
			if (IsInTriangle(point, v1, v2, v3))
			{
				result.emplace_back(point);
			}
		}

		return result;
	}

	bool IsConvex(Ref<Point> point)
	{
		Point a = *point->previous;
		Point b = *point;
		Point c = *point->next;

		auto res = ((a.position.x * (c.position.z - b.position.z)) + (b.position.x * (a.position.z - c.position.z)) + (c.position.x * (b.position.z - a.position.z)));
		return res > 0;
	}

	Ref<Point> GetFarthestPointFromLine(const std::vector<Ref<Point>>& pointsInTriangle, Ref<Point> startLine, Ref<Point> endLine)
	{
		Ref<Point> farthestPoint;
		float farthestDistance = 0.f;

		gem::vec3 line = endLine->position - startLine->position;

		for (const auto& point : pointsInTriangle)
		{
			if (point->index == startLine->index || point->index == endLine->index || point->index == startLine->previous->index) { continue; }

			auto startToPoint = point->position - startLine->position;
			auto pointToLine = startToPoint - line * gem::dot(startToPoint, line) / gem::dot(line, line);
			auto magnitude = gem::length(pointToLine);
			if (magnitude >= farthestDistance)
			{
				farthestPoint = point;
				farthestDistance = magnitude;
			}
		}

		return farthestPoint;
	}

	std::pair<Polygon, Polygon> SplitPolygon(const Polygon& polygon, Ref<Point> startLine, Ref<Point> endLine)
	{
		std::pair<Polygon, Polygon> result;
		result.first = polygon;
		result.second = polygon.ClonePolygon();

		auto startLine2 = result.second.GetPointFromIndex(startLine->index);
		auto endLine2 = result.second.GetPointFromIndex(endLine->index);

		while (startLine->next->index != endLine->index)
		{
			result.first.RemovePointWithIndex(startLine->next->index);
		}

		while (startLine2->previous->index != endLine2->index)
		{
			result.second.RemovePointWithIndex(startLine2->previous->index);
		}

		return result;
	}

	std::vector<MeshIndex> TriangulatePolygon(const Polygon& polygon)
	{
		// #SAMUEL_TODO: This crashes on polygons with holes 

		std::vector<MeshIndex> indices;

		for (const auto& p : polygon.points)
		{
			// Check that polygon has enough verts to make triangle
			if (polygon.GetVertexCount() == 3)
			{
				// Successful triangle, add it to indices
				indices.push_back(p->previous->index);
				indices.push_back(p->index);
				indices.push_back(p->next->index);
				break;
			}
			else if (IsConvex(p)) // If first vertex is convex
			{
				auto pointsInTri = GetPointsInTriangle(polygon, p, p->next, p->previous);
				if (pointsInTri.size() == 0)
				{
					// Successful triangle, add it to indices
					indices.push_back(p->previous->index);
					indices.push_back(p->index);
					indices.push_back(p->next->index);

					Polygon newPolygon = polygon;

					// remove added triangle and repeat for the rest
					newPolygon.RemovePointWithIndex(p->index);

					auto result = TriangulatePolygon(newPolygon);
					indices.insert(indices.end(), result.begin(), result.end());
				}
				else
				{
					// Split lists and rerun triangulate
					auto splitPolygons =
						SplitPolygon(polygon, p, GetFarthestPointFromLine(pointsInTri, p->next, p->previous));
					auto result = TriangulatePolygon(splitPolygons.first);
					auto result2 = TriangulatePolygon(splitPolygons.second);

					// remove dublicates after this if needed
					indices.insert(indices.end(), result.begin(), result.end());
					indices.insert(indices.end(), result2.begin(), result2.end());
				}
				break;
			}
		}

		return indices;
	}

	struct Triangle
	{
		bool operator==(const Triangle& rhs)
		{
			return (
				indices[0] == rhs.indices[0] &&
				indices[1] == rhs.indices[1] &&
				indices[2] == rhs.indices[2]);
		}

		std::array<uint32_t, 3> indices;
	};

	Polygon CreatePolygon(const MeshInfo& meshInfo)
	{
		Polygon poly;

		const auto& verts = meshInfo.first;
		const auto& indices = meshInfo.second;

		if (verts.empty() || indices.empty()) { return poly; }

		// Convert indices to triangles
		std::vector<Triangle> triangles;

		for (uint32_t i = 0; i < indices.size(); i += 3)
		{
			Triangle tri;
			tri.indices[0] = indices[i];
			tri.indices[1] = indices[i + 1];
			tri.indices[2] = indices[i + 2];
			triangles.push_back(tri);
		}

		// Check how many triangle each edge is connected to
		std::unordered_map<uint64_t, uint32_t> edgeTriCount;
		for (uint32_t i = 0; i < triangles.size(); i++)
		{
			for (uint32_t j = 0; j < 3; j++)
			{
				uint32_t nextIndex = (j + 1 < 3) ? j + 1 : 0;
				uint64_t edge = ((uint64_t)(triangles[i].indices[j]) << 32) | triangles[i].indices[nextIndex];
				uint64_t edgeFlipped = ((uint64_t)(triangles[i].indices[nextIndex]) << 32) | triangles[i].indices[j];

				if (edgeTriCount.find(edgeFlipped) != edgeTriCount.end())
				{
					edgeTriCount[edgeFlipped] += 1;
				}
				else
				{
					edgeTriCount[edge] += 1;
				}
			}
		}

		std::vector<std::pair<MeshIndex, MeshIndex>> outerEdges;

		// Add edge points to polygon
		for (const auto& edge : edgeTriCount)
		{
			auto edgeKey = edge.first;
			if (edge.second == 1)
			{
				auto edgeStartIndex = (uint32_t)(edgeKey >> 32);
				auto edgeEndIndex = (uint32_t)(edgeKey);

				outerEdges.push_back(std::pair<MeshIndex, MeshIndex>(edgeStartIndex, edgeEndIndex));
				Ref<Point> p = CreateRef<Point>(edgeStartIndex, verts[edgeStartIndex].position);
				poly.points.emplace_back(p);
			}
		}

		// Set point relationships
		for (uint32_t i = 0; i < poly.points.size(); i++)
		{
			auto currentIndex = poly.points[i]->index;
			for (uint32_t j = 0; j < outerEdges.size(); j++)
			{
				auto startIndex = outerEdges[j].first;
				auto endIndex = outerEdges[j].second;
				if (startIndex == currentIndex)
				{
					poly.points[i]->next = poly.GetPointFromIndex(endIndex);
				}
				else if (endIndex == currentIndex)
				{
					poly.points[i]->previous = poly.GetPointFromIndex(startIndex);
				}
			}
		}

		return poly;
	}

	bool MergePolygons(const Polygon& a, const Polygon& b, Polygon& outPoly)
	{
		Polygon result = a.ClonePolygon();
		Polygon copy = b.ClonePolygon();

		bool successfulMerge = false;

		result.ForEach([&](Ref<Point> point)
			{
				if (!point || !point->next || !point->previous) { return; }
				Line pointLine(gem::vec2(point->position.x, point->position.z), gem::vec2(point->next->position.x, point->next->position.z));

				copy.ForEach([&](Ref<Point> point2)
					{
						if (point == point2) { return; }
						if (!point2 || !point2->next || !point2->previous) { return; }

						Line point2Line(gem::vec2(point2->position.x, point2->position.z), gem::vec2(point2->next->position.x, point2->next->position.z));

						float mergeDistance = 50.f;
						auto distance = gem::dot(pointLine.GetNormal(), point2Line.GetMiddle() - pointLine.GetMiddle());

						if (pointLine.Facing(point2Line) && distance < mergeDistance)
						{
							point->next->previous = point2;
							point2->next->previous = point;

							auto pN = point->next;
							point->next = point2->next;
							point2->next = pN;

							successfulMerge = true;
						}
					});
			});

		if (successfulMerge)
		{
			result.points.insert(result.points.end(), copy.points.begin(), copy.points.end());
			outPoly = result;
		}

		return successfulMerge;
	}

	Polygon MergePolygons(const std::vector<Polygon>& polygons)
	{
		auto polygonList = polygons;
		Polygon result = polygonList[0];

		bool updateResult = true;
		std::vector<uint32_t> mergeIndices;
		mergeIndices.emplace_back(0);

		// Update Indices
		int offset = 0;
		for (auto& poly : polygonList)
		{
			poly.ForEach([&](Ref<Point> p)
				{
					p->index += offset;
				});
			offset += (int32_t)poly.points.size();
		}

		while (updateResult)
		{
			updateResult = false;

			for (uint32_t i = 0; i < polygonList.size(); i++)
			{
				if (std::find(mergeIndices.begin(), mergeIndices.end(), i) != mergeIndices.end()) { continue; }

				if (MergePolygons(result, polygonList[i], result))
				{
					mergeIndices.emplace_back(i);
					updateResult = true;
				}
			}
		}

		// Validate Poly
		{
			auto startIndex = result.points[0]->index;
			auto current = result.points[0]->next;
			for (uint32_t i = 0; i < result.points.size(); i++)
			{
				if (i == result.points.size() - 1 && current->index == startIndex)
				{
					VT_INFO("[EDGE MERGE]: Successfully merged edges");
					break;
				}
				else if (current->index == startIndex)
				{
					VT_WARN(std::string("[EDGE MERGE]: Failed, loop ended at index ") + std::to_string(i) + std::string(" but should have ended at ") + std::to_string(result.points.size() - 1));
					break;
				}
				current = current->next;
			}
		}

		return result;
	}
}

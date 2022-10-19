#pragma once
#include <gem/gem.h>
#include <vector>
#include <stack>

#include <Volt/Rendering/Vertex.h>

#define VertsPerPoly 3

namespace Volt
{
	typedef uint32_t CellID;
	constexpr CellID NullCell = 0;
	typedef uint32_t MeshIndex;
	typedef std::pair<std::vector<Vertex>, std::vector<MeshIndex>> MeshInfo;

	struct NavMeshVertex
	{
		NavMeshVertex(const Vertex& aVertex)
		{
			Position = aVertex.position;
		};
		gem::vec3 Position = { 0.f, 0.f, 0.f };
	};

	struct NavMeshCell
	{
		CellID Id = 0;
		uint32_t numNeighbours = 0;
		gem::vec3 Center = { 0.f, 0.f, 0.f };
		std::array<CellID, VertsPerPoly> NeighbourCells = { NullCell, NullCell, NullCell };
		std::array<gem::vec3, VertsPerPoly> NeighbourPortals = { gem::vec3(0.f), gem::vec3(0.f), gem::vec3(0.f) };
		std::array<MeshIndex, VertsPerPoly> Indices = { 0, 0, 0 };
	};

	struct NavMesh
	{
		std::vector<MeshIndex> Indices;
		std::vector<NavMeshVertex> Vertices;
		std::vector<NavMeshCell> Cells;
	};

	struct NavMeshPath
	{
		std::stack<CellID> PathIds;
		std::stack<gem::vec3> PathPositions;
	};
}
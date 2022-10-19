#pragma once
#include "Volt/Core/Base.h"
#include "Volt/AI/NavMesh/NavMeshData.h"
#include "Volt/AI/NavMesh/NavigationsSystem.h"

namespace Volt
{
	class AStar
	{
	public:
		AStar() = delete;
		~AStar() = delete;

		static NavMeshPath FindPath(gem::vec3 current, gem::vec3 target);
		static CellID GetCellIDFromPosition(gem::vec3 position);

	private:
		struct PathfinderNode
		{
			CellID id = 0;
			float cost = 0.f;
			float heuristic = 0.f;
			bool visited = false;
			Ref<PathfinderNode> prevNode;
			std::vector<Ref<PathfinderNode>> connections;
		};

	private:
		static gem::vec3 GetPortal(CellID aFrom, CellID aTo);
		static std::vector<Ref<PathfinderNode>> ConvertCellsToNodes(CellID aStart, CellID aTarget);
	};
}
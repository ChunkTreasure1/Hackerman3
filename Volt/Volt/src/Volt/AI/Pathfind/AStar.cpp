#include "vtpch.h"
#include "AStar.h"

namespace Volt
{
	CellID AStar::GetCellIDFromPosition(gem::vec3 position)
	{
		CellID id = NullCell;

		auto navmesh = NavigationsSystem::GetNavMesh();

		for (const auto& cell : navmesh.Cells)
		{
			gem::vec3 p = position;
			gem::vec3 a = navmesh.Vertices[cell.Indices[0]].Position;
			gem::vec3 b = navmesh.Vertices[cell.Indices[1]].Position;
			gem::vec3 c = navmesh.Vertices[cell.Indices[2]].Position;

			a -= p;
			b -= p;
			c -= p;

			p -= p;

			gem::vec3 u = gem::cross(b, c);
			gem::vec3 v = gem::cross(c, a);
			gem::vec3 w = gem::cross(a, b);

			if (gem::dot(u, v) < 0.f)
			{
				continue;
			}
			if (gem::dot(u, w) < 0.0f)
			{
				continue;
			}

			id = cell.Id;
			break;
		}
		return id;
	}

	NavMeshPath TempPath()
	{
		NavMeshPath FinalPath;

		auto& navmesh = NavigationsSystem::GetNavMesh();
		auto& cellList = navmesh.Cells;

		if (!cellList.empty())
		{
			// THIS IS GONNA GET CHANGED AND IS JUST FOR TESTING RN
			NavMeshCell searchCell;

			for (const auto& cell : cellList)
			{
				if (cell.numNeighbours == 1 && FinalPath.PathIds.empty())
				{
					FinalPath.PathIds.push(cell.Id);
					FinalPath.PathPositions.push(cellList[cell.Id - 1].NeighbourPortals[0]);
					searchCell = cellList[cell.NeighbourCells[0] - 1];
					break;
				}
			}

			CellID lastVisited = FinalPath.PathIds.top();
			while (searchCell.numNeighbours != 1)
			{
				for (uint32_t i = 0; i < searchCell.NeighbourCells.size(); i++)
				{
					if (searchCell.NeighbourCells[i] != lastVisited && searchCell.NeighbourCells[i] != 0)
					{
						FinalPath.PathIds.push(searchCell.NeighbourCells[i]);
						FinalPath.PathPositions.push(cellList[searchCell.Id - 1].NeighbourPortals[i]);
						lastVisited = searchCell.Id;
						searchCell = cellList[searchCell.NeighbourCells[i] - 1];
						break;
					}
				}
			}
		}

		return FinalPath;
	}

	NavMeshPath AStar::FindPath(gem::vec3 current, gem::vec3 target)
	{
		NavMeshPath FinalPath;
		bool pathFound = false;

		auto& navmesh = NavigationsSystem::GetNavMesh();

		auto startCell = GetCellIDFromPosition(current);
		auto endCell = GetCellIDFromPosition(target);

		if (startCell == NullCell || endCell == NullCell)
		{
			return FinalPath;
		}

		auto nodes = ConvertCellsToNodes(startCell, endCell);

		Ref<PathfinderNode> currentNode;
		std::vector<Ref<PathfinderNode>> priorityQueue;

		for (const auto& n : nodes)
		{
			if (n->id == startCell)
			{
				currentNode = n;
				currentNode->cost = 0;
				break;
			}
		}

		priorityQueue.emplace_back(currentNode);

		while (!pathFound)
		{
			for (const auto& node : currentNode->connections)
			{
				const auto& x = navmesh.Cells[node->id - 1];
				const auto& y = navmesh.Cells[currentNode->id - 1];
				auto newCost = currentNode->cost + gem::distance(x.Center, y.Center);
				if (newCost < node->cost)
				{
					node->cost = newCost;
					node->prevNode = currentNode;
					priorityQueue.emplace_back(node);
					std::sort(priorityQueue.begin(), priorityQueue.end(), [](Ref<PathfinderNode> x, Ref<PathfinderNode> y)
						{
							return (x->cost + x->heuristic) < (y->cost + y->heuristic);
						});
				}
			}

			for (const auto& node : priorityQueue)
			{
				if (!node->visited)
				{
					currentNode = node;
				}
			}

			if (currentNode->visited)
			{
				return FinalPath;
			}

			currentNode->visited = true;
			if (currentNode->id == endCell)
			{
				pathFound;
				break;
			}
		}

		while (currentNode->prevNode)
		{
			FinalPath.PathIds.push(currentNode->id);
			FinalPath.PathPositions.push(GetPortal(currentNode->id, currentNode->prevNode->id));
			currentNode = currentNode->prevNode;
		}

		return FinalPath;
	}

	gem::vec3 AStar::GetPortal(CellID aFrom, CellID aTo)
	{
		auto& navmesh = NavigationsSystem::GetNavMesh();

		auto from = navmesh.Cells[aFrom - 1];
		auto to = navmesh.Cells[aTo - 1];

		for (uint32_t i = 0; i < from.NeighbourCells.size(); i++)
		{
			if (from.NeighbourCells[i] == aTo)
			{
				return from.NeighbourPortals[i];
			}
		}
		return gem::vec3(0.f, 0.f, 0.f);
	}

	std::vector<Ref<AStar::PathfinderNode>> AStar::ConvertCellsToNodes(CellID aStart, CellID aTarget)
	{
		auto result = std::vector<Ref<PathfinderNode>>();
		auto connectionsMap = std::unordered_map<CellID, std::vector<Ref<PathfinderNode>>>();

		auto& navmesh = NavigationsSystem::GetNavMesh();

		for (const auto& cell : navmesh.Cells)
		{
			Ref<PathfinderNode> node = CreateRef<PathfinderNode>();
			node->id = cell.Id;
			node->cost = INT_MAX;
			node->heuristic = gem::distance(navmesh.Cells[aTarget - 1].Center, cell.Center);
			node->visited = false;
			node->prevNode = nullptr;

			for (const auto& neighbour : cell.NeighbourCells)
			{
				connectionsMap[neighbour].emplace_back(node);
			}

			result.emplace_back(node);
		}

		for (auto& node : result)
		{
			node->connections = connectionsMap.find(node->id)->second;
		}

		return result;
	}
}
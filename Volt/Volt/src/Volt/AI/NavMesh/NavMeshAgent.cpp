#include "vtpch.h"
#include "NavMeshAgent.h"

#include "Volt/Components/Components.h"
#include "Volt/AI/Pathfind/AStar.h"

namespace Volt
{
	NavMeshAgent::NavMeshAgent()
	{

	}

	NavMeshAgent::~NavMeshAgent()
	{

	}

	void NavMeshAgent::StartNavigation()
	{
		myActive = true;
	}

	void NavMeshAgent::StopNavigation()
	{
		myActive = false;
	}

	void NavMeshAgent::SetTarget(gem::vec3 aPosition)
	{
		// #SAMUEL_TODO: Conversion to get closest point on the navmesh from aPosition

		myTarget = aPosition;
		myPath = FindPath(myCurrent, myTarget);
	}

	void NavMeshAgent::Update(float aTimestep, TransformComponent& aTransformComp)
	{
		if (myActive)
		{
			myCurrent = aTransformComp.position;
			MoveToTarget(aTimestep, aTransformComp);
		}
	}

	void NavMeshAgent::MoveToTarget(float aTimestep, TransformComponent& aTransformComp)
	{
		gem::vec3 targetPos;
		if (!myPath.empty()) 
		{
			targetPos = myPath.top();
		}
		else
		{
			targetPos = myTarget; // #SAMUEL_TODO: Add check so this isn't out of navmesh
		}
		
		auto moveDirection = gem::normalize(targetPos - aTransformComp.position);
		if (gem::distance(aTransformComp.position, targetPos) > 10.f)
		{
			aTransformComp.position += moveDirection * mySpeed * aTimestep;
		}
		else if (!myPath.empty())
		{
			myPath.pop();
		}
	}

	std::stack<gem::vec3> NavMeshAgent::FindPath(const gem::vec3& start, const gem::vec3& target)
	{
		auto res = AStar::FindPath(start, target);
		return res.PathPositions;
	}
}

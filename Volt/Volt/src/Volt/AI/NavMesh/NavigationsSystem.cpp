#include "vtpch.h"
#include "NavigationsSystem.h"

#include "Volt/Components/Components.h"

namespace Volt
{
	NavigationsSystem::NavigationsSystem(Ref<Scene>& aScene) 
		: myCurrentScene(aScene)
	{

	}

	NavigationsSystem::~NavigationsSystem()
	{

	}

	void NavigationsSystem::OnRuntimeUpdate(float aTimestep)
	{
		myCurrentScene->GetRegistry().ForEach<NavMeshAgentComponent, TransformComponent>([&](Wire::EntityId id, NavMeshAgentComponent& agentComp, TransformComponent& transformComp)
			{
				if (!test)
				{
					agentComp.agent.Update(aTimestep, transformComp);
					agentComp.agent.SetTarget(gem::vec3(590.f, 0.f, 18.f));
					test = true;
				}
				agentComp.agent.Update(aTimestep, transformComp);
			});
	}

	void NavigationsSystem::OnRuntimeStop()
	{
		test = false;
	}
}

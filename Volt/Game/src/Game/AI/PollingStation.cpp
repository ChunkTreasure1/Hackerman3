#include "PollingStation.h"

#include "CompterScript.h"

#include <Volt/Scene/Scene.h>
#include <Volt/Core/Base.h>
#include <Volt/Components/Components.h>

PollingStation::PollingStation(Volt::Scene* scene)
	: myScene(scene)
{
	VT_CORE_ASSERT(myInstance == nullptr, "Instance already exists!");
	myInstance = this;
}

PollingStation::~PollingStation()
{
	myInstance = nullptr;
}

void PollingStation::Update(float aDeltaTime)
{
	bool changed = false;
	myScene->GetRegistry().ForEach<CompterComponent, Volt::TransformComponent>([&](Wire::EntityId id, const CompterComponent& comp, const Volt::TransformComponent& transComp) 
		{
			if (comp.isPlayerInTrigger)
			{
				changed = true;
				myCurrentHackResult.isHacked = true;
				myCurrentHackResult.target = transComp.position;
			}

			if (!changed)
			{
				myCurrentHackResult.isHacked = false;
			}
		});
}

const HackPollResult& PollingStation::PollIsBeingHacked() const
{
	return myCurrentHackResult;
}

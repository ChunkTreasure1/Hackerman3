#include "PollingStation.h"

#include "WanderController.h"
#include "SeekController.h"
#include "SeparationController.h"

#include <Volt/Scene/Scene.h>
#include <Volt/Core/Base.h>
#include <Volt/Components/Components.h>

PollingStation::PollingStation(Volt::Scene* scene)
	: myScene(scene)
{
	VT_CORE_ASSERT(myInstance == nullptr, "Instance already exists!");
	myInstance = this;

	myControllerPostitions.reserve(20);
}

PollingStation::~PollingStation()
{
	myInstance = nullptr;
}

void PollingStation::Update(float aDeltaTime)
{
	bool changed = false;
	myControllerPostitions.clear();

	myScene->GetRegistry().ForEach<WanderControllerComponent, Volt::TransformComponent>([&](Wire::EntityId id, const WanderControllerComponent& comp, const Volt::TransformComponent& transComp)
		{
			myTargetPosition = transComp.position;
			myControllerPostitions.emplace_back(Volt::Entity{ id, myScene });
		});

	myScene->GetRegistry().ForEach<SeekControllerComponent, Volt::TransformComponent>([&](Wire::EntityId id, const SeekControllerComponent& comp, const Volt::TransformComponent& transComp) 
		{
			myControllerPostitions.emplace_back(Volt::Entity{ id, myScene });
		});

	myScene->GetRegistry().ForEach<SeperationControllerComponent, Volt::TransformComponent>([&](Wire::EntityId id, const SeperationControllerComponent& comp, const Volt::TransformComponent& transComp)
		{
			myControllerPostitions.emplace_back(Volt::Entity{ id, myScene });
		});
}

const gem::vec3& PollingStation::PollTargetPosition() const
{
	return myTargetPosition;
}
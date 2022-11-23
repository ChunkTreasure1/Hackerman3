#include "PollingStationU4.h"

#include "Game/AI-U4/AIU4Controller.h"

#include <Volt/Scene/Scene.h>
#include <Volt/Core/Base.h>
#include <Volt/Components/Components.h>

PollingStationU4::PollingStationU4(Volt::Scene* scene)
	: myScene(scene)
{
	VT_CORE_ASSERT(myInstance == nullptr, "Instance already exists!");
	myInstance = this;

	myControllerPostitions.reserve(20);
}

PollingStationU4::~PollingStationU4()
{
	myInstance = nullptr;
}

void PollingStationU4::Update(float aDeltaTime)
{
	bool changed = false;
	myControllerPostitions.clear();

	myScene->GetRegistry().ForEach<AIU4ControllerComponent, Volt::TransformComponent>([&](Wire::EntityId id, const AIU4ControllerComponent& comp, const Volt::TransformComponent& transComp)
		{
			myControllerPostitions.emplace_back(Volt::Entity{ id, myScene });
		});
}

const gem::vec3& PollingStationU4::PollTargetPosition() const
{
	return myTargetPosition;
}
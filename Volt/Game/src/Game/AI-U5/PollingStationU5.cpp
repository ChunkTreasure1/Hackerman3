#include "PollingStationU5.h"

#include "StateMachineActor.h"
#include "DecisionTreeActor.h"

PollingStationU5::PollingStationU5(Volt::Scene* scene)
	: myScene(scene)
{
	myInstance = this;
}

PollingStationU5::~PollingStationU5()
{
	myInstance = nullptr;
}

void PollingStationU5::Update(float aDeltaTime)
{
	myScene->GetRegistry().ForEach<AIU5StateActorComponent>([&](Wire::EntityId id, AIU5StateActorComponent&) 
		{
			myStateTargetPosition = Volt::Entity{ id, myScene }.GetPosition();
		});

	myScene->GetRegistry().ForEach<AIU5DecisionActorComponent>([&](Wire::EntityId id, AIU5DecisionActorComponent&)
		{
			myDecisionTargetPosition = Volt::Entity{ id, myScene }.GetPosition();
		});
}

const gem::vec3& PollingStationU5::PollStateTargetPosition() const
{
	return myStateTargetPosition;
}

const gem::vec3& PollingStationU5::PollDecisionTargetPosition() const
{
	return myDecisionTargetPosition;
}

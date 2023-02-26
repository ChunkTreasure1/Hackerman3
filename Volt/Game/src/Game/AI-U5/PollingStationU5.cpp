#include "PollingStationU5.h"

#include "StateMachineActor.h"
#include "DecisionTreeActor.h"

PollingStationU5::PollingStationU5(Volt::Scene* scene)
	: myScene(scene)
{
	myInstance = this;
	mySafeDist = 5000;
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

const gem::vec3& PollingStationU5::PollDecisionRespawnPosition() const
{
	float dist = gem::distance(myDecisionRespawnPosition, myStateTargetPosition);

	if (dist < mySafeDist)
	{
		return myStateRespawnPosition;
	}
	else
	{
		return myDecisionRespawnPosition;
	}
}

const gem::vec3& PollingStationU5::PollStateRespawnPosition() const
{
	float dist = gem::distance(myStateRespawnPosition, myDecisionTargetPosition);

	if (dist < mySafeDist)
	{
		return myDecisionRespawnPosition;
	}
	else
	{
		return myStateRespawnPosition;
	}
}

void PollingStationU5::SetDecisionRespawn(const gem::vec3& aPos)
{
	myDecisionRespawnPosition = aPos;
}

void PollingStationU5::SetStateRespawn(const gem::vec3& aPos)
{
	myStateRespawnPosition = aPos;
}

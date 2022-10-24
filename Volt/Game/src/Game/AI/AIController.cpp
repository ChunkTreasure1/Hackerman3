#include "AIController.h"

#include "PollingStation.h"

#include <Volt/Components/Components.h>

VT_REGISTER_SCRIPT(AIController)

AIController::AIController(Volt::Entity entity)
	: Volt::ScriptBase(entity)
{}

void AIController::OnStart()
{
	AIEventManager::Get().Register(this);
}

void AIController::OnUpdate(float aDeltaTime)
{
	constexpr float walkSpeed = 300.f;

	if (myEntity.GetComponent<AIControllerComponent>().isPolling)
	{
		const HackPollResult& result = PollingStation::Get().PollIsBeingHacked();
		if (result.isHacked)
		{
			float distToTarget = gem::length(result.target - myEntity.GetPosition());
			if (distToTarget > 40)
			{
				gem::vec3 dir = gem::normalize(result.target - myEntity.GetPosition());
				const gem::vec3 pos = myEntity.GetPosition() + dir * walkSpeed * aDeltaTime;

				myEntity.SetPosition({ pos.x, myEntity.GetPosition().y, pos.z });
			}
		}
	}
	else
	{
		if (myHasTarget)
		{
			float distToTarget = gem::length(myTargetPosition - myEntity.GetPosition());
			if (distToTarget > 40)
			{
				gem::vec3 dir = gem::normalize(myTargetPosition - myEntity.GetPosition());
				const gem::vec3 pos = myEntity.GetPosition() + dir * walkSpeed * aDeltaTime;

				myEntity.SetPosition({ pos.x, myEntity.GetPosition().y, pos.z });
			}
		}
	}
}

void AIController::OnAIEvent(AIEvent event, Volt::Entity entity)
{
	if (myEntity.GetComponent<AIControllerComponent>().isPolling)
	{
		return;
	}

	switch (event)
	{
		case AIEvent::PanikCompterHak:
			myTargetPosition = entity.GetPosition();
			myHasTarget = true;
			break;

		case AIEvent::HakStop:
			myHasTarget = false;
			break;
	}
}

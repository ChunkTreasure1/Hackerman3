#include "SeparationController.h"

#include "PollingStation.h"

VT_REGISTER_SCRIPT(SeparationController);

SeparationController::SeparationController(Volt::Entity entity)
	: Volt::ScriptBase(entity)
{}

void SeparationController::OnStart()
{}

void SeparationController::OnUpdate(float aDeltaTime)
{
	float minDistance = FLT_MAX;
	gem::vec3 minTargetPosition = 0.f;

	const auto& sepComp = myEntity.GetComponent<SeperationControllerComponent>();
	const auto& controllerPositions = PollingStation::Get().PollControllerPositions();
	for (const auto& ent : controllerPositions) 
	{
		const float dist = gem::distance(ent.GetPosition(), myEntity.GetPosition());
		if (dist < sepComp.threshold && dist < minDistance && myEntity != ent)
		{
			minDistance = dist;
			minTargetPosition = ent.GetPosition();
		}
	}

	if (minDistance == FLT_MAX)
	{
		return;
	}

	const float strength = gem::min(sepComp.speedDecay / (minDistance * minDistance), sepComp.speed);
	const gem::vec3 direction = gem::normalize(myEntity.GetPosition() - minTargetPosition);
	
	myEntity.SetPosition(myEntity.GetPosition() + gem::vec3{ direction.x, 0.f, direction.z } * sepComp.speed * aDeltaTime);
	
	auto rot = gem::eulerAngles(gem::quatLookAt(direction, { 0.f, 1.f, 0.f }));
	myEntity.SetRotation({ 0.f, rot.y, 0.f });

	auto currPos = myEntity.GetPosition();
	if (currPos.x > 2500.f)
	{
		myEntity.SetPosition({ -2500.f, currPos.y, currPos.z });
	}
	else if (currPos.x < -2500.f)
	{
		myEntity.SetPosition({ 2500.f, currPos.y, currPos.z });
	}
	else if (currPos.z > 2500.f)
	{
		myEntity.SetPosition({ currPos.x, currPos.y, -2500.f });
	}
	else if (currPos.z < -2500.f)
	{
		myEntity.SetPosition({ currPos.x, currPos.y, 2500.f });
	}
}
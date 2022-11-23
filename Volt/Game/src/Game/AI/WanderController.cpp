#include "WanderController.h"

#include <Volt/Utility/Random.h>

VT_REGISTER_SCRIPT(WanderController)

WanderController::WanderController(Volt::Entity entity)
	: Volt::ScriptBase(entity)
{}

void WanderController::OnStart()
{}

void WanderController::OnUpdate(float aDeltaTime)
{
	const auto& wanderComp = myEntity.GetComponent<WanderControllerComponent>();
	if (myTimer <= 0.f)
	{
		myTimer = wanderComp.cooldown;

		myTargetYRotation = myEntity.GetRotation().y + gem::radians(Volt::Random::Float(wanderComp.minRotation, wanderComp.maxRotation));
	}
	else
	{
		myTimer -= aDeltaTime;
	}

	myEntity.SetPosition(myEntity.GetPosition() + myEntity.GetForward() * wanderComp.speed * aDeltaTime);

	const float rotDir = myEntity.GetRotation().y - myTargetYRotation > 0 ? 1.f : -1.f;
	myEntity.SetRotation({ 0.f, myEntity.GetRotation().y + gem::radians(wanderComp.rotationSpeed) * rotDir * aDeltaTime, 0.f });

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
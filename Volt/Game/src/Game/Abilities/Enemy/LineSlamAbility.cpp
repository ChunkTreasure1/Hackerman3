#include "LineSlamAbility.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include <Volt/Components/Components.h>

void LineSlamAbility::Cast()
{
	if (!IsCastable())
		return;
	const auto& data = AbilityData::LineSlamData;
	myCooldownTimer = data->cooldown;

	auto ent = myEntity.GetScene()->CreateEntity();
	auto& transform = ent.GetComponent<Volt::TransformComponent>();
	transform.rotation = myEntity.GetWorldRotation(); //.GetComponent<Volt::TransformComponent>().rotation;
	transform.position = myEntity.GetWorldPosition();//.GetComponent<Volt::TransformComponent>().position;
	ent.AddScript("LineSlamAbilityScript");
}
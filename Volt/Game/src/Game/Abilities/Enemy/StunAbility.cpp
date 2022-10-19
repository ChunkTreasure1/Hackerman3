#include "StunAbility.h"
#include "Game/Abilities/Enemy/StunAbility.h"
#include "Game/Abilities/Scripts/StunAbilityScript.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include <Volt/Components/Components.h>

void StunAbility::Cast()
{
	if (myCooldownTimer >= 0)
		return;// maybe some pop up to indicate a failure to cast
	myCooldownTimer = AbilityData::StunAttackData->cooldown;
	auto ent = myEntity.GetScene()->CreateEntity();

	auto& transform = ent.GetComponent<Volt::TransformComponent>();
	transform.rotation = myEntity.GetComponent<Volt::TransformComponent>().rotation;
	transform.position = myEntity.GetComponent<Volt::TransformComponent>().position;
	ent.AddScript("StunAbilityScript");
	auto* script = ent.GetScript<StunAbilityScript>("StunAbilityScript");
}

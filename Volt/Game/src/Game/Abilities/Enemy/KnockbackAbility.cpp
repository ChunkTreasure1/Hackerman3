#include "KnockbackAbility.h"
#include "Game/Abilities/Scripts/KnockbackAbilityScript.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include <Volt/Components/Components.h>

void KnockbackAbility::Cast()
{
	if (myCooldownTimer >= 0)
		return;// maybe some pop up to indicate a failure to cast
	myCooldownTimer = AbilityData::CloseAttackData->cooldown;
	auto ent = myEntity.GetScene()->CreateEntity();

	auto& transform = ent.GetComponent<Volt::TransformComponent>();
	transform.rotation = myEntity.GetComponent<Volt::TransformComponent>().rotation;
	transform.position = myEntity.GetComponent<Volt::TransformComponent>().position;
	ent.AddScript("KncokbackAbilityScript");
}

#include "MeleeAbility.h"
#include "Game/Abilities/Enemy/MeleeAbility.h"
#include "Game/Abilities/Scripts/MeleeAbilityScript.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include <Volt/Components/Components.h>

void MeleeAbility::Cast()
{
	if (myCooldownTimer >= 0)
		return;// maybe some pop up to indicate a failure to cast
	myCooldownTimer = AbilityData::CloseAttackData->cooldown;
	auto ent = myEntity.GetScene()->CreateEntity();

	ent.SetRotation(myEntity.GetComponent<Volt::TransformComponent>().rotation);
	ent.SetPosition(myEntity.GetComponent<Volt::TransformComponent>().position);
	ent.AddScript("MeleeAbilityScript");
	auto* script = ent.GetScript<MeleeAbilityScript>("MeleeAbilityScript");
}

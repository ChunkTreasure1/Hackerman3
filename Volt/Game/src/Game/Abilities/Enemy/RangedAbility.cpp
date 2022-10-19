#include "RangedAbility.h"
#include "Game/Abilities/Enemy/RangedAbility.h"
#include "Game/Abilities/Scripts/RangedAbilityScript.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include <Volt/Components/Components.h>
void RangedAbility::Cast()
{
	if (myCooldownTimer >= 0)
		return;// maybe some pop up to indicate a failure to cast
	myCooldownTimer = AbilityData::RangedAttackData->cooldown;
	auto ent = myEntity.GetScene()->CreateEntity();

	ent.SetRotation(myEntity.GetComponent<Volt::TransformComponent>().rotation);
	auto t = myEntity.GetComponent<Volt::TransformComponent>().position;
	t.y += 100;
	ent.SetWorldPosition(t);
	ent.AddScript("RangedAbilityScript");
}

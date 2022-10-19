#include "GasOffensiveAbility.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include <Volt/Components/Components.h>

void GasOffensiveAbility::Cast()
{

}

void GasOffensiveAbility::CastAt(gem::vec3 aPos)
{
	if (myCooldownTimer >= 0)
		return;// maybe some pop up to indicate a failure to cast
	myCooldownTimer = AbilityData::GasAttackData->cooldown;
	auto ent = myEntity.GetScene()->CreateEntity();

	auto& transform = ent.GetComponent<Volt::TransformComponent>();
	transform.position = aPos;
	ent.AddScript("GasOffensiveAbilityScript");
}


#include "GasShieldAbility.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include <Volt/Components/Components.h>

void GasShieldAbility::Cast()
{
	myShieldEntity = myEntity.GetScene()->CreateEntity();

	auto& transform = myShieldEntity.GetComponent<Volt::TransformComponent>();
	myEntity.GetScene()->ParentEntity(myEntity, myShieldEntity);
	myShieldEntity.SetPosition(myEntity.GetWorldPosition());
	myShieldEntity.AddScript("GasShieldAbilityScript");
}

void GasShieldAbility::Cancel()
{
	myEntity.GetScene()->RemoveEntity(myShieldEntity);
}

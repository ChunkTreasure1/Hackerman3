#include "PlayerBloodlustAbility.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Components/Components.h"
#include "Volt/Components/PhysicsComponents.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Log/Log.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include "GEM/gem.h"
#include "Game/Player/PlayerScript.h"

void PlayerBloodlustAbility::Cast()
{
	myEntity.GetScript<PlayerScript>("PlayerScript")->ActivateBloodlust();

	myEntity.GetComponent<Volt::PlayerComponent>().currentFury -= AbilityData::BloodLustData->cost;

	SetCooldownTimer(AbilityData::BloodLustData->cooldown);
	myEntity.GetScript<PlayerScript>("PlayerScript")->CanCancelAbility(true);
}
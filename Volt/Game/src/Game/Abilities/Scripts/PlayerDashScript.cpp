#include "PlayerDashScript.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Log/Log.h"
#include "Volt/Scripting/ScriptBase.h"
#include "Game/Abilities/AbilityDescriptions.hpp"

VT_REGISTER_SCRIPT(PlayerDashScript)

PlayerDashScript::PlayerDashScript(const Volt::Entity& aEntity)
	:ScriptBase(aEntity)
{
}

void PlayerDashScript::OnTriggerEnter(Volt::Entity entity, bool isTrigger)
{
	if (entity.HasComponent<Volt::HealthComponent>() && !entity.HasComponent<Volt::PlayerComponent>())
	{
		entity.GetComponent<Volt::HealthComponent>().health -= AbilityData::DashData->damage;
	}
}

void PlayerDashScript::OnAwake()
{
	myEntity.GetScene()->RemoveEntity(myEntity, 0.2f);
}

void PlayerDashScript::OnUpdate(float aDeltaTime)
{
}

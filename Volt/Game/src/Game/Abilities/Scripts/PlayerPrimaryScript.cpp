#include "PlayerPrimaryScript.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Components/Components.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Log/Log.h"
#include "Volt/Scripting/ScriptBase.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include "Game/Player/PlayerScript.h"
#include "Volt/Components/PhysicsComponents.h"

VT_REGISTER_SCRIPT(PlayerPrimaryScript)

PlayerPrimaryScript::PlayerPrimaryScript(const Volt::Entity& aEntity)
	:ScriptBase(aEntity)
{
}

void PlayerPrimaryScript::OnTriggerEnter(Volt::Entity entity, bool isTrigger)
{
	if (entity.HasComponent<Volt::HealthComponent>() && !entity.HasComponent<Volt::PlayerComponent>() && entity.GetComponent<Volt::RigidbodyComponent>().layerId != 4)
	{
		Volt::Entity tempEnt;
		myEntity.GetScene()->GetRegistry().ForEach<Volt::PlayerComponent>([&](Wire::EntityId id, const Volt::PlayerComponent& scriptComp)
			{
				tempEnt = { id, myEntity.GetScene() };
				tempEnt.GetComponent<Volt::PlayerComponent>().currentFury += AbilityData::PrimaryData->furyIncrease;
				tempEnt.GetScript<PlayerScript>("PlayerScript")->ResetFuryTimer();
			});
		entity.GetComponent<Volt::HealthComponent>().health -= AbilityData::PrimaryData->damage;
	}
} 

void PlayerPrimaryScript::OnAwake()
{
	myEntity.GetScene()->RemoveEntity(myEntity, 0.1f);
}

void PlayerPrimaryScript::OnUpdate(float aDeltaTime)
{
	
}
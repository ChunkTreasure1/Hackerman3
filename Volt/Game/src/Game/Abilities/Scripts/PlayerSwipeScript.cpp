#include "PlayerSwipeScript.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Components/Components.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Log/Log.h"
#include "Volt/Scripting/ScriptBase.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include "Game/Events/DestructibleObject.h"
#include "Volt/Components/PhysicsComponents.h"

VT_REGISTER_SCRIPT(PlayerSwipeScript)

PlayerSwipeScript::PlayerSwipeScript(const Volt::Entity& aEntity)
	:ScriptBase(aEntity)
{
}

void PlayerSwipeScript::OnTriggerEnter(Volt::Entity entity, bool isTrigger)
{
	if (entity.HasComponent<Volt::HealthComponent>() && !entity.HasComponent<Volt::PlayerComponent>() && entity.GetComponent<Volt::RigidbodyComponent>().layerId != 4)
	{
		entity.GetComponent<Volt::HealthComponent>().health -= AbilityData::SwipeData->damage;
	}
}

void PlayerSwipeScript::OnAwake()
{
	myEntity.GetScene()->RemoveEntity(myEntity, 0.1f);
}

void PlayerSwipeScript::OnUpdate(float aDeltaTime)
{
}

#include "PlayerArrowBuffedScript.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Log/Log.h"
#include "Volt/Scripting/ScriptBase.h"
#include "Game/Abilities/AbilityDescriptions.hpp"

VT_REGISTER_SCRIPT(PlayerArrowBuffedScript)

PlayerArrowBuffedScript::PlayerArrowBuffedScript(const Volt::Entity& aEntity)
	:ScriptBase(aEntity)
{
}

void PlayerArrowBuffedScript::OnTriggerEnter(Volt::Entity entity, bool isTrigger)
{
	if (entity.HasComponent<Volt::HealthComponent>())
	{
		entity.GetComponent<Volt::HealthComponent>().health -= AbilityData::FearData->damage;
	}
}

void PlayerArrowBuffedScript::OnAwake()
{
	myEntity.GetScene()->RemoveEntity(myEntity, AbilityData::FearData->fearTime);
}

void PlayerArrowBuffedScript::OnUpdate(float aDeltaTime)
{
	if (myEntity.GetScale().x < 3)
	{
		myEntity.SetScale({ myEntity.GetScale().x + 0.15f, 0.1f , myEntity.GetScale().z + 0.15f });
	}
}

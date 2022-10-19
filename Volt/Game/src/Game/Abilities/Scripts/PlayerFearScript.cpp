#include "PlayerFearScript.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Log/Log.h"
#include "Volt/Scripting/ScriptBase.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include "Game/Enemy/Bruiser/BruiserEnemyScript.h"
#include "Game/Enemy/Ranged/RangedEnemyScript.h"
#include "Game/Enemy/Melee/MeleeEnemyScript.h"


VT_REGISTER_SCRIPT(PlayerFearScript)

PlayerFearScript::PlayerFearScript(const Volt::Entity& aEntity)
	:ScriptBase(aEntity)
{
	
}

void PlayerFearScript::OnTriggerEnter(Volt::Entity entity, bool isTrigger)
{
	if (entity.HasComponent<Volt::HealthComponent>() && !entity.HasComponent<Volt::PlayerComponent>())
	{
		if (entity.HasComponent<Volt::MeleeEnemyComponent>())
		{
			//entity.GetScript<BruiserEnemyScript>("BruiserEnemyScript")->Fear();
		}
		else if (entity.HasComponent<Volt::RangedEnemyComponent>())
		{
			//entity.GetScript<RangedEnemyScript>("RangedEnemyScript")->Fear();
		}
		else if (entity.HasComponent<Volt::BruiserEnemyComponent>())
		{
			//entity.GetScript<MeleeEnemyScript>("MeleeEnemyScript")->Fear();
		}
		entity.GetComponent<Volt::HealthComponent>().health -= AbilityData::FearData->damage;
	}
}

void PlayerFearScript::OnAwake()
{
	myInterpolation = 0;
	myEntity.SetScale({ 0.1f,0.1f,0.1f });
	myEntity.GetScene()->RemoveEntity(myEntity, AbilityData::FearData->fearTime);
}

void PlayerFearScript::OnUpdate(float aDeltaTime)
{
	if (myEntity.GetScale().x < AbilityData::FearData->radius)
	{
		myEntity.SetScale({ myEntity.GetScale().x + 0.15f, myEntity.GetScale().y , myEntity.GetScale().z + 0.15f });
	}
}

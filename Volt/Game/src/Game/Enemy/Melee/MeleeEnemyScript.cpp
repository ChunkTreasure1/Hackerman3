#include "MeleeEnemyScript.h"
#include <Volt/Components/Components.h>
#include <Volt/Components/PhysicsComponents.h>
#include <Volt/Scripting/ScriptRegistry.h>
#include "Game/Abilities/Enemy/MeleeAbility.h"
#include <Volt/Asset/AssetManager.h>

VT_REGISTER_SCRIPT(MeleeEnemyScript);

MeleeEnemyScript::MeleeEnemyScript(const Volt::Entity& aEntity) : ScriptBase(aEntity)
{}

void MeleeEnemyScript::OnAwake()
{
	myEntity.SetWorldPosition({ myEntity.GetComponent<Volt::TransformComponent>().position.x, myEntity.GetComponent<Volt::TransformComponent>().position.y, myEntity.GetComponent<Volt::TransformComponent>().position.z + 0.000000001f });
	myEntity.GetComponent<Volt::TagComponent>().tag = "Melee Enemy";

	auto& t = myEntity.AddComponent<Volt::AnimatedCharacterComponent>();
	t.animatedCharacter = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/AI/Enemies/melee_enemy/CHR_MeleeEnemy.vtchr");
	t.isLooping = false;
	
	mySM = CreateRef<MeleeEnemySM>(myEntity);
	mySM->CreateStates();
}

void MeleeEnemyScript::OnStart()
{
	auto testvar = myEntity.GetComponent<Volt::MeleeEnemyComponent>().buffedHealthIncrease;
	// TODO: SHADER MAGIC
	if (myEntity.GetComponent<Volt::MeleeEnemyComponent>().isBuffed)
	{
		myEntity.GetComponent<Volt::HealthComponent>().health = myEntity.GetComponent<Volt::HealthComponent>().health * myEntity.GetComponent<Volt::MeleeEnemyComponent>().buffedHealthIncrease;
	}
	else if (myEntity.GetComponent<Volt::MeleeEnemyComponent>().isChampion)
	{
		myEntity.GetComponent<Volt::HealthComponent>().health = myEntity.GetComponent<Volt::HealthComponent>().health * myEntity.GetComponent<Volt::MeleeEnemyComponent>().buffedHealthIncrease;
	}
	myEntity.GetComponent<Volt::HealthComponent>().maxHealth = myEntity.GetComponent<Volt::HealthComponent>().health;

}


void MeleeEnemyScript::OnUpdate(float aDeltaTime)
{
	if (myEntity.GetComponent<Volt::HealthComponent>().health <= 0)
	{
		myEntity.GetScene()->RemoveEntity(myEntity, 0.001f);
		return;
	}
	else
		mySM->Update(aDeltaTime);
}

void MeleeEnemyScript::OnStop()
{}
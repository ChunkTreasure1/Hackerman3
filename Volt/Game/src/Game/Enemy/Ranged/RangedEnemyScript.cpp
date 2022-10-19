#include "RangedEnemyScript.h"
#include <Volt/Components/Components.h>
#include <Volt/Scripting/ScriptRegistry.h>
#include "Game/Abilities/Enemy/RangedAbility.h"
#include <Volt/Asset/AssetManager.h>

VT_REGISTER_SCRIPT(RangedEnemyScript);

RangedEnemyScript::RangedEnemyScript(const Volt::Entity& aEntity) : ScriptBase(aEntity)
{}

void RangedEnemyScript::OnAwake()
{
	myEntity.SetWorldPosition({ myEntity.GetComponent<Volt::TransformComponent>().position.x, myEntity.GetComponent<Volt::TransformComponent>().position.y, myEntity.GetComponent<Volt::TransformComponent>().position.z + 0.000000001f });
	myEntity.GetComponent<Volt::TagComponent>().tag = "Ranged Enemy";
	//myEntity.AddComponent<Volt::MeshComponent>().handle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Animations/Gremlin/gremlin_sk.vtmesh");

	auto& t = myEntity.AddComponent<Volt::AnimatedCharacterComponent>();
	t.animatedCharacter = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/AI/Enemies/ranged_enemy/CHR_RangedEnemy.vtchr");
	t.isLooping = false;

	mySM = CreateRef<RangedEnemySM>(myEntity);
	mySM->CreateStates();
}

void RangedEnemyScript::OnStart()
{
	// TODO: SHADER MAGIC
	if (myEntity.GetComponent<Volt::RangedEnemyComponent>().isBuffed)
	{
		auto testvar = myEntity.GetComponent<Volt::RangedEnemyComponent>().buffedHealthIncrease;
		myEntity.GetComponent<Volt::HealthComponent>().health = myEntity.GetComponent<Volt::HealthComponent>().health * myEntity.GetComponent<Volt::RangedEnemyComponent>().buffedHealthIncrease;
	}
	myEntity.GetComponent<Volt::HealthComponent>().maxHealth = myEntity.GetComponent<Volt::HealthComponent>().health;
}


void RangedEnemyScript::OnUpdate(float aDeltaTime)
{
	if (myEntity.GetComponent<Volt::HealthComponent>().health <= 0)
	{
		myEntity.GetScene()->RemoveEntity(myEntity, 0.001f);
		return;
	}
	else
		mySM->Update(aDeltaTime);
}

void RangedEnemyScript::OnStop()
{}
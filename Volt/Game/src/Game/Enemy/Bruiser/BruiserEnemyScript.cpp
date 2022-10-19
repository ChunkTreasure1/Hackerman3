#include "BruiserEnemyScript.h"
#include <Volt/Components/Components.h>
#include <Volt/Scripting/ScriptRegistry.h>
#include "Game/Abilities/Enemy/StunAbility.h"
#include <Volt/Asset/AssetManager.h>

VT_REGISTER_SCRIPT(BruiserEnemyScript);

BruiserEnemyScript::BruiserEnemyScript(const Volt::Entity& aEntity) : ScriptBase(aEntity)
{}

void BruiserEnemyScript::OnAwake()
{
	myEntity.SetWorldPosition({ myEntity.GetComponent<Volt::TransformComponent>().position.x, myEntity.GetComponent<Volt::TransformComponent>().position.y, myEntity.GetComponent<Volt::TransformComponent>().position.z + 0.000000001f });
	myEntity.GetComponent<Volt::TagComponent>().tag = "Bruiser Enemy";

	auto& t = myEntity.AddComponent<Volt::AnimatedCharacterComponent>();
	t.animatedCharacter = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/AI/Enemies/bruiser_enemy/CHR_Bruiser.vtchr");
	t.isLooping = false;

	mySM = CreateRef<BruiserEnemySM>(myEntity);
	mySM->CreateStates();
}

void BruiserEnemyScript::OnStart()
{
	// TODO: SHADER MAGIC
	if (myEntity.GetComponent<Volt::BruiserEnemyComponent>().isBuffed)
	{
		myEntity.GetComponent<Volt::HealthComponent>().health = myEntity.GetComponent<Volt::HealthComponent>().health * myEntity.GetComponent<Volt::BruiserEnemyComponent>().buffedHealthIncrease;
	}
	myEntity.GetComponent<Volt::HealthComponent>().maxHealth = myEntity.GetComponent<Volt::HealthComponent>().health;
}


void BruiserEnemyScript::OnUpdate(float aDeltaTime)
{
	if (myEntity.GetComponent<Volt::HealthComponent>().health <= 0)
	{
		myEntity.GetScene()->RemoveEntity(myEntity, 0.001f);
		return;
	}
	else
		mySM->Update(aDeltaTime);
}

void BruiserEnemyScript::OnStop()
{}
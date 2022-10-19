#include "BossScript.h"
#include <Volt/Components/Components.h>	
#include <Volt/Asset/AssetManager.h>

VT_REGISTER_SCRIPT(BossScript);

BossScript::BossScript(const Volt::Entity& aEntity) : ScriptBase(aEntity)
{}

BossScript::~BossScript()
{}

void BossScript::OnAwake()
{
	myEntity.GetComponent<Volt::TransformComponent>().position.z += 0.000000001f;
	mySM = CreateRef<BossSM>(myEntity);
	mySM->CreateStates();
	myEntity.GetComponent<Volt::TagComponent>().tag = "Boss";
	auto& t = myEntity.AddComponent<Volt::AnimatedCharacterComponent>();
	t.animatedCharacter = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Animations/King/CHR_King.vtchr");
	t.isLooping = false;
	
	/*auto abtEnt = myEntity.GetScene()->CreateEntity();
	abtEnt.GetScene()->ParentEntity(myEntity, abtEnt);
	abtEnt.GetComponent<Volt::TagComponent>().tag = "Ability Entity";*/
	myAbilityHandler = CreateRef<AbilityHandler>(myEntity);

	myStartHP = myEntity.GetComponent<Volt::HealthComponent>().health;
	myEntity.GetComponent<Volt::HealthComponent>().maxHealth = myEntity.GetComponent<Volt::HealthComponent>().health;
}

void BossScript::OnStart()
{}

void BossScript::OnUpdate(float deltaTime)
{
	mySM->Update(deltaTime);
	myAbilityHandler->Update(deltaTime);
}

void BossScript::OnStop()
{}

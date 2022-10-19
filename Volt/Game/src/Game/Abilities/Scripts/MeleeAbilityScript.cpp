#include "MeleeAbilityScript.h"
#include <Volt/Components/Components.h>
#include <Volt/Components/PhysicsComponents.h>
#include <Volt/Scripting/ScriptRegistry.h>
#include <Volt/Log/Log.h>
#include "Game/Abilities/AbilityDescriptions.hpp"
#include <Volt/Asset/AssetManager.h>

VT_REGISTER_SCRIPT(MeleeAbilityScript);

MeleeAbilityScript::MeleeAbilityScript(const Volt::Entity& aEntity) : ScriptBase(aEntity)
{
	myTimer = 0;
	myTheTimeMyEnitityHaveUntilAnInevitablyPainfulDemise = 0;
	myRange = 0;
	myWidth = 0;
	myDirecton = { 0 };
}

void MeleeAbilityScript::OnAwake()
{
	auto& data = AbilityData::CloseAttackData;
	myTimer = 0;
	myWidth = data->size.x;
	myRange = data->size.z;
	myTheTimeMyEnitityHaveUntilAnInevitablyPainfulDemise = data->timeAlive;

	myEntity.GetComponent<Volt::TagComponent>().tag = "MeleeAbility - Hitbox";
	auto& transform = myEntity.GetComponent<Volt::TransformComponent>();
	transform.position += myEntity.GetForward() * (myRange * 0.5f);
	transform.scale = { myWidth * 0.01f,AbilityData::CloseAttackData->size.y * 0.01f,myRange * 0.01f };

	/*auto& bx = myEntity.AddComponent<Volt::BoxColliderComponent>();
	bx.isTrigger = true;
	auto& rigid = myEntity.AddComponent<Volt::RigidbodyComponent>();
	rigid.isKinematic = true;
	rigid.layerId = 5;*/
	auto& bx = myEntity.AddComponent<Volt::BoxColliderComponent>();
	bx.isTrigger = true;
	Volt::RigidbodyComponent rig{};
	rig.isKinematic = true;
	rig.layerId = 5;
	auto& rigid = myEntity.AddComponent<Volt::RigidbodyComponent>(&rig);
}

void MeleeAbilityScript::OnStart()
{}

void MeleeAbilityScript::OnUpdate(float aDeltaTime)
{
	myTimer += aDeltaTime;
	if (myTimer >= myTheTimeMyEnitityHaveUntilAnInevitablyPainfulDemise)
		myEntity.GetScene()->RemoveEntity(myEntity);
}

void MeleeAbilityScript::OnStop()
{}

void MeleeAbilityScript::OnTriggerEnter(Volt::Entity entity, bool isTrigger)
{
	if (entity.HasComponent<Volt::HealthComponent>() && entity.HasComponent<Volt::PlayerComponent>())
	{
		entity.GetComponent<Volt::HealthComponent>().health -= AbilityData::CloseAttackData->damage;
		myEntity.GetScene()->RemoveEntity(myEntity, 0.001f);
	}
}

#include "RangedAbilityScript.h"
#include <Volt/Components/Components.h>
#include <Game/Abilities/AbilityDescriptions.hpp>
#include <Volt/Asset/AssetManager.h>
#include <Volt/Components/PhysicsComponents.h>

VT_REGISTER_SCRIPT(RangedAbilityScript)

RangedAbilityScript::RangedAbilityScript(const Volt::Entity& aEntity) : ScriptBase(aEntity)
{}

void RangedAbilityScript::OnAwake()
{
	auto& data = AbilityData::RangedAttackData;

	auto& mesh = myEntity.AddComponent<Volt::MeshComponent>();
	mesh.handle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Meshes/Primitives/Cube.vtmesh");
	myEntity.GetComponent<Volt::TagComponent>().tag = "RangedAbility";

	auto& transform = myEntity.GetComponent<Volt::TransformComponent>();
	myEntity.SetWorldPosition(myEntity.GetWorldPosition() + myEntity.GetForward() * (AbilityData::RangedAttackData->size.z * 0.5f));
	myDistanceTraveled = (AbilityData::RangedAttackData->size.z * 0.5f);
	myEntity.SetScale({ AbilityData::RangedAttackData->size.x * 0.01f,AbilityData::RangedAttackData->size.y * 0.01f,AbilityData::RangedAttackData->size.z * 0.01f });

	auto& bx = myEntity.AddComponent<Volt::BoxColliderComponent>();
	bx.isTrigger = true;
	Volt::RigidbodyComponent rig{};
	rig.isKinematic = true;
	rig.layerId = 5;
	auto& rigid = myEntity.AddComponent<Volt::RigidbodyComponent>(&rig);
}

void RangedAbilityScript::OnStart()
{}

void RangedAbilityScript::OnUpdate(float aDeltaTime)
{
	myDistanceTraveled += aDeltaTime * AbilityData::RangedAttackData->speed;
	myEntity.SetWorldPosition(myEntity.GetWorldPosition() + myEntity.GetWorldForward() * aDeltaTime * AbilityData::RangedAttackData->speed);
	if (myDistanceTraveled >= AbilityData::RangedAttackData->range)
		myEntity.GetScene()->RemoveEntity(myEntity);
}

void RangedAbilityScript::OnStop()
{}

void RangedAbilityScript::OnTriggerEnter(Volt::Entity entity, bool isTrigger)
{
	if (entity.HasComponent<Volt::HealthComponent>() && entity.HasComponent<Volt::PlayerComponent>())
	{
		entity.GetComponent<Volt::HealthComponent>().health -= AbilityData::RangedAttackData->damage;
		myEntity.GetScene()->RemoveEntity(myEntity, 0.001f);
	}
}

#include "StunAbilityScript.h"
#include <Volt/Components/Components.h>
#include <Volt/Components/PhysicsComponents.h>
#include <Volt/Scripting/ScriptRegistry.h>
#include <Volt/Log/Log.h>
#include "Game/Abilities/AbilityDescriptions.hpp"
#include <Volt/Asset/AssetManager.h>

VT_REGISTER_SCRIPT(StunAbilityScript);

StunAbilityScript::StunAbilityScript(const Volt::Entity& aEntity) : ScriptBase(aEntity)
{
	myTimer = 0;
	myTheTimeMyEnitityHaveUntilAnInevitablyPainfulDemise = 0;
	myRange = 0;
	myWidth = 0;
}

void StunAbilityScript::OnAwake()
{
	auto& data = AbilityData::StunAttackData;
	myTimer = 0;
	myWidth = data->size.x;
	myRange = data->size.z;
	myTheTimeMyEnitityHaveUntilAnInevitablyPainfulDemise = data->timeAlive;


	auto& mesh = myEntity.AddComponent<Volt::MeshComponent>();
	mesh.handle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Meshes/Primitives/Cube.vtmesh");
	myEntity.GetComponent<Volt::TagComponent>().tag = "StunAbility";
	auto& transform = myEntity.GetComponent<Volt::TransformComponent>();
	transform.position += myEntity.GetForward() * (myRange * 0.5f);
	transform.scale = { myWidth * 0.01f,data->size.y * 0.01f,myRange * 0.01f };

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

void StunAbilityScript::OnStart()
{}

void StunAbilityScript::OnUpdate(float aDeltaTime)
{
	myTimer += aDeltaTime;
	if (myTimer >= myTheTimeMyEnitityHaveUntilAnInevitablyPainfulDemise)
		myEntity.GetScene()->RemoveEntity(myEntity);
}

void StunAbilityScript::OnStop()
{}

void StunAbilityScript::OnTriggerEnter(Volt::Entity entity, bool isTrigger)
{
	if (entity.HasComponent<Volt::HealthComponent>() && entity.HasComponent<Volt::PlayerComponent>())
	{
		entity.GetComponent<Volt::HealthComponent>().health -= AbilityData::StunAttackData->damage;
		myEntity.GetScene()->RemoveEntity(myEntity, 0.001f);
	}
}

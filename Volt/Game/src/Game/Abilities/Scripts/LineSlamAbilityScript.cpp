#include "LineSlamAbilityScript.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include <Volt/Components/Components.h>
#include <Volt/Components/PhysicsComponents.h>
#include <Volt/Asset/AssetManager.h>

VT_REGISTER_SCRIPT(LineSlamAbilityScript);

LineSlamAbilityScript::LineSlamAbilityScript(const Volt::Entity& aEntity) : ScriptBase(aEntity)
{
	myTimer = 0;
}

void LineSlamAbilityScript::OnAwake()
{
	const auto& data = AbilityData::LineSlamData;

	myTimer = 0;
	auto& mesh = myEntity.AddComponent<Volt::MeshComponent>().handle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Meshes/Primitives/Cube.vtmesh");
	myEntity.GetComponent<Volt::TagComponent>().tag = "LineSlam Hitbox";
	auto& transform = myEntity.GetComponent<Volt::TransformComponent>();

	transform.position += myEntity.GetForward() * data->size.z * 0.5f;
	transform.scale = { data->size.x * 0.01f, data->size.y * 0.01f, data->size.z * 0.01f };

	auto& bx = myEntity.AddComponent<Volt::BoxColliderComponent>();
	bx.isTrigger = true;
	auto& rigid = myEntity.AddComponent<Volt::RigidbodyComponent>();
	rigid.isKinematic = true;
	rigid.layerId = 5;
}

void LineSlamAbilityScript::OnStart()
{

}

void LineSlamAbilityScript::OnUpdate(float aDeltaTime)
{
	myTimer += aDeltaTime;
	if (myTimer >= AbilityData::LineSlamData->colliderLifetime)
	{
		myEntity.GetScene()->RemoveEntity(myEntity);
		return;
	}
}

void LineSlamAbilityScript::OnStop()
{

}


void LineSlamAbilityScript::OnTriggerEnter(Volt::Entity entity, bool isTrigger)
{
	if (entity.HasComponent<Volt::HealthComponent>())
	{
		// GET DATA FROM ABILITYDATA THING when added
		entity.GetComponent<Volt::HealthComponent>().health -= 10;
		myEntity.GetScene()->RemoveEntity(myEntity, 0.001f);
	}
}

#include "PlayerArrowScript.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Components/Components.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Log/Log.h"
#include "Volt/Scripting/ScriptBase.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include "Volt/Components/PhysicsComponents.h"
#include "Game/Events/DestructibleObject.h"

VT_REGISTER_SCRIPT(PlayerArrowScript)

PlayerArrowScript::PlayerArrowScript(const Volt::Entity& aEntity)
	:ScriptBase(aEntity)
{}

void PlayerArrowScript::OnTriggerEnter(Volt::Entity entity, bool isTrigger)
{
	if (entity.HasComponent<Volt::HealthComponent>() && !entity.HasComponent<Volt::PlayerComponent>())
	{
		Volt::Entity tempEnt;
		myEntity.GetScene()->GetRegistry().ForEach<Volt::PlayerComponent>([&](Wire::EntityId id, const Volt::PlayerComponent& scriptComp)
			{
				tempEnt = { id, myEntity.GetScene() };
				tempEnt.GetComponent<Volt::PlayerComponent>().currentFury += AbilityData::ArrowData->furyIncrease;
			});
		entity.GetComponent<Volt::HealthComponent>().health -= AbilityData::ArrowData->damage;
	}

	if (myIsBuffed)
	{
		SpawnAOE();
	}

	if (entity.GetComponent<Volt::RigidbodyComponent>().layerId != 4)
	{
		myEntity.GetScene()->RemoveEntity(myEntity);
	}
}

void PlayerArrowScript::SpawnAOE()
{
	Volt::Entity ent = myEntity.GetScene()->CreateEntity();

	ent.AddComponent<Volt::MeshComponent>().handle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Meshes/Primitives/Capsule.vtmesh");
	ent.SetPosition(myEntity.GetPosition());

	auto& coll = ent.AddComponent<Volt::CapsuleColliderComponent>();
	coll.isTrigger = true;

	ent.AddComponent<Volt::RigidbodyComponent>();
	ent.AddScript("PlayerArrowBuffedScript");
}

void PlayerArrowScript::OnAwake()
{
	myEntity.GetScene()->RemoveEntity(myEntity, AbilityData::ArrowData->lifetime);
}

void PlayerArrowScript::OnUpdate(float aDeltaTime)
{
	myEntity.SetPosition(myEntity.GetPosition() + myEntity.GetForward() * AbilityData::ArrowData->speed * aDeltaTime);
}

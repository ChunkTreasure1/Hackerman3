#include "PlayerArrowAbility.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Components/Components.h"
#include "Volt/Components/PhysicsComponents.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Log/Log.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include "GEM/gem.h"
#include "Game/Abilities/Scripts/PlayerArrowScript.h"
#include "Game/Player/PlayerScript.h"
#include "Volt/Asset/Prefab.h"

void PlayerArrowAbility::SetDir(gem::vec3 aPosition)
{
	myShootDirection = gem::normalize(aPosition - myEntity.GetPosition());

	auto& transform = myEntity.GetComponent<Volt::TransformComponent>();

	gem::mat test = gem::lookAtLH(transform.position, aPosition, { 0,1,0 });
	gem::vec3 rot = 0;
	gem::vec3 dump = 0;
	gem::decompose(test, dump, rot, dump);
	transform.rotation = rot;
	transform.rotation.y *= -1.0f;
}

void PlayerArrowAbility::Cast()
{
	//Volt::Entity ent = myEntity.GetScene()->CreateEntity();
	
	auto prefab = Volt::AssetManager::GetAsset<Volt::Prefab>("Assets/Prefabs/Arrow.vtprefab");

	Wire::EntityId id = prefab->Instantiate(myEntity.GetScene()->GetRegistry());
	Volt::Entity arrow{ id, myEntity.GetScene() };

	/*ent.AddComponent<Volt::MeshComponent>().handle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Meshes/Primitives/Cube.vtmesh");
	ent.SetScale({ 0.2f,0.2f,0.6f });*/
	arrow.SetPosition(myEntity.GetPosition() + myShootDirection * 3);
	arrow.SetPosition({ arrow.GetPosition().x,arrow.GetPosition().y + 180, arrow.GetPosition().z });
	arrow.SetRotation(myEntity.GetRotation());

	

	auto& coll = arrow.AddComponent<Volt::BoxColliderComponent>();
	coll.isTrigger = true;
	coll.halfSize.y = 200;

	Volt::RigidbodyComponent rigidInit{};
	rigidInit.isKinematic = true;
	rigidInit.layerId = 2;
	auto& rb = arrow.AddComponent<Volt::RigidbodyComponent>(&rigidInit);
	arrow.AddScript("PlayerArrowScript");

	if (myEntity.GetComponent<Volt::PlayerComponent>().isBuffed)
	{
		arrow.GetScript<PlayerArrowScript>("PlayerArrowScript")->IsBuffed(true);
	}
	myCooldownTimer = AbilityData::ArrowData->cooldown;
	myEntity.GetScript<PlayerScript>("PlayerScript")->CanCancelAbility(true);
}
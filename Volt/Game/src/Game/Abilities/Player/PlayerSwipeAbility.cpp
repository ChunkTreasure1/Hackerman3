#include "PlayerSwipeAbility.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Components/Components.h"
#include "Volt/Components/PhysicsComponents.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Log/Log.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include "GEM/gem.h"
#include "Game/Player/PlayerScript.h"

void PlayerSwipeAbility::SetDir(gem::vec3 aPosition)
{
	myAttackDir = gem::normalize(aPosition - myEntity.GetPosition());

	gem::mat test = gem::lookAtLH(myEntity.GetPosition(), aPosition, { 0,1,0 });
	gem::vec3 rot = 0;
	gem::vec3 dump = 0;
	gem::decompose(test, dump, rot, dump);
	myEntity.SetRotation({rot.x, rot.y * -1.0f, rot.z});
}

void PlayerSwipeAbility::Cast()
{
	Volt::Entity ent = myEntity.GetScene()->CreateEntity();

	//ent.AddComponent<Volt::MeshComponent>().handle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Meshes/Primitives/Cube.vtmesh");
	ent.SetScale(AbilityData::SwipeData->size);
	ent.SetPosition(myEntity.GetPosition() + myAttackDir * 150);
	ent.SetRotation(myEntity.GetRotation());

	auto& coll = ent.AddComponent<Volt::BoxColliderComponent>();
	coll.isTrigger = true;

	Volt::RigidbodyComponent rigidInit{};
	rigidInit.isKinematic = true;
	rigidInit.layerId = 2;
	auto& rb = ent.AddComponent<Volt::RigidbodyComponent>(&rigidInit);
	ent.AddScript("PlayerSwipeScript");

	myCooldownTimer = AbilityData::SwipeData->cooldown;
	myEntity.GetScript<PlayerScript>("PlayerScript")->CanCancelAbility(true);
}

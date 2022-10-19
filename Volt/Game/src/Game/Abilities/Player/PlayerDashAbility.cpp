#include "PlayerDashAbility.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Components/Components.h"
#include "Volt/Components/PhysicsComponents.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Log/Log.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include "GEM/gem.h"
#include "Game/Player/PlayerScript.h"

void PlayerDashAbility::SetDir(gem::vec3 aPosition)
{
	myDashDirection = aPosition - myEntity.GetPosition();

	gem::mat test = gem::lookAtLH(myEntity.GetPosition(), aPosition, { 0,1,0 });
	gem::vec3 rot = 0;
	gem::vec3 dump = 0;
	gem::decompose(test, dump, rot, dump);
	myEntity.SetRotation(rot);
	myEntity.SetRotation({ rot.x, rot.y * -1.0f, rot.z });
}


void PlayerDashAbility::Cast()
{
	Volt::Entity ent = myEntity.GetScene()->CreateEntity();

	ent.AddComponent<Volt::MeshComponent>().handle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Meshes/Primitives/Cube.vtmesh");

	if (gem::length(myDashDirection) / 100 < AbilityData::DashData->range)
	{
		ent.SetScale({ 0.5f,2.0f, gem::length(myDashDirection) / 100 });
	}
	else
	{
		ent.SetScale({ 0.5f,2.0f, AbilityData::DashData->range});
	}

	ent.SetPosition(myEntity.GetPosition() + myEntity.GetForward() * (ent.GetScale().z * 0.5f) * 100);
	ent.SetRotation(myEntity.GetRotation());

	myEntity.SetPosition(myEntity.GetPosition() + myEntity.GetForward() * (ent.GetScale().z) * 100);

	auto& coll = ent.AddComponent<Volt::BoxColliderComponent>();
	coll.isTrigger = true;

	Volt::RigidbodyComponent rigidInit{};
	rigidInit.isKinematic = true;
	rigidInit.layerId = 2;
	auto& rb = ent.AddComponent<Volt::RigidbodyComponent>(&rigidInit);

	ent.AddScript("PlayerDashScript");

	myCooldownTimer = AbilityData::DashData->cooldown;
	myEntity.GetScript<PlayerScript>("PlayerScript")->CanCancelAbility(true);
}

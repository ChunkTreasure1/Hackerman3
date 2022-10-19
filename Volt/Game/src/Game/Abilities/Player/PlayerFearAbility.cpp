#include "PlayerFearAbility.h"
#include "Volt/Components/Components.h"
#include "Volt/Components/PhysicsComponents.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Log/Log.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include "GEM/gem.h"
#include "Game/Player/PlayerScript.h"

void PlayerFearAbility::Cast()
{
	Volt::Entity ent = myEntity.GetScene()->CreateEntity();

	ent.AddComponent<Volt::MeshComponent>().handle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Meshes/Primitives/Capsule.vtmesh");
	ent.SetPosition(myEntity.GetPosition());
	ent.SetRotation(myEntity.GetRotation());

	auto& coll = ent.AddComponent<Volt::CapsuleColliderComponent>();
	coll.isTrigger = true;

	auto& rb = ent.AddComponent<Volt::RigidbodyComponent>();
	rb.isKinematic = true;
	rb.layerId = 1;

	ent.AddScript("PlayerFearScript");

	myCooldownTimer = AbilityData::FearData->cooldown;
	myEntity.GetScript<PlayerScript>("PlayerScript")->CanCancelAbility(true);
}

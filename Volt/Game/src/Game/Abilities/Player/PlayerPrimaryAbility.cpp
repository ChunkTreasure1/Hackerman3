#include "PlayerPrimaryAbility.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Components/Components.h"
#include "Volt/Components/PhysicsComponents.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Log/Log.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include "GEM/gem.h"
#include "Game/Player/PlayerScript.h"

void PlayerPrimaryAbility::SetDir(gem::vec3 aPosition)
{
	myAttackDir = gem::normalize(aPosition - myEntity.GetPosition());

	auto& transform = myEntity.GetComponent<Volt::TransformComponent>();

	gem::mat test = gem::lookAtLH(transform.position, aPosition, { 0,1,0 });
	gem::vec3 rot = 0;
	gem::vec3 dump = 0;
	gem::decompose(test, dump, rot, dump);
	transform.rotation = rot;
	transform.rotation.y *= -1.0f;
}
void PlayerPrimaryAbility::Cast()
{
	Volt::Entity ent = myEntity.GetScene()->CreateEntity();

	//ent.AddComponent<Volt::MeshComponent>().handle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Meshes/Primitives/Cube.vtmesh");
	ent.SetScale(AbilityData::PrimaryData->size);
	ent.SetPosition(myEntity.GetPosition() + myAttackDir * 150);
	ent.SetRotation(myEntity.GetRotation());

	auto& coll = ent.AddComponent<Volt::BoxColliderComponent>();
	coll.isTrigger = true;

	Volt::RigidbodyComponent rigidInit{};
	rigidInit.isKinematic = true;
	rigidInit.layerId = 2;
	auto& rigidComp = ent.AddComponent<Volt::RigidbodyComponent>(&rigidInit);
	
	ent.AddScript("PlayerPrimaryScript");

	myCooldownTimer = AbilityData::PrimaryData->cooldown;
	myEntity.GetScript<PlayerScript>("PlayerScript")->CanCancelAbility(true);	
}
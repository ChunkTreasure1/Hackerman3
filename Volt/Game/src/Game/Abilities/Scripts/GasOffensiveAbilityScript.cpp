#include "GasOffensiveAbilityScript.h"
#include <Volt/Components/Components.h>
#include <Volt/Asset/AssetManager.h>
#include "Game/Abilities/AbilityDescriptions.hpp"

VT_REGISTER_SCRIPT(GasOffensiveAbilityScript);

GasOffensiveAbilityScript::GasOffensiveAbilityScript(const Volt::Entity& aEntity) : ScriptBase(aEntity)
{}

void GasOffensiveAbilityScript::OnAwake()
{

	myEntity.GetComponent<Volt::TagComponent>().tag = "Gas attack";
	myEntity.AddComponent<Volt::MeshComponent>().handle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Meshes/Primitives/Cylinder.vtmesh");
	myEntity.SetScale({ AbilityData::GasAttackData->radius  * 0.02f,20 * 0.01f , AbilityData::GasAttackData->radius * 0.02f});
}

void GasOffensiveAbilityScript::OnStart()
{}

void GasOffensiveAbilityScript::OnUpdate(float aDeltaTime)
{}

void GasOffensiveAbilityScript::OnStop()
{}

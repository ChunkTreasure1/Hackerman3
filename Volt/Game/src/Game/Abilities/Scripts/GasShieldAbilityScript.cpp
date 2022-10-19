#include "GasShieldAbilityScript.h"
#include <Volt/Components/Components.h>
#include <Volt/Asset/AssetManager.h>
#include "Game/Abilities/AbilityDescriptions.hpp"

VT_REGISTER_SCRIPT(GasShieldAbilityScript);

GasShieldAbilityScript::GasShieldAbilityScript(const Volt::Entity& aEntity) : ScriptBase(aEntity)
{}

void GasShieldAbilityScript::OnAwake()
{
	myEntity.GetComponent<Volt::TagComponent>().tag = "Gas shield";
	myEntity.AddComponent<Volt::MeshComponent>().handle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Meshes/Primitives/Cylinder.vtmesh");
	myEntity.SetScale({ AbilityData::GasShieldData->radius * 0.02f,20 * 0.01f , AbilityData::GasShieldData->radius * 0.02f });
}

void GasShieldAbilityScript::OnStart()
{}

void GasShieldAbilityScript::OnUpdate(float aDeltaTime)
{}

void GasShieldAbilityScript::OnStop()
{}

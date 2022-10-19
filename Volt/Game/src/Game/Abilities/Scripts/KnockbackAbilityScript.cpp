#include "KnockbackAbilityScript.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include <Volt/Components/Components.h>
#include <Volt/Asset/AssetManager.h>

VT_REGISTER_SCRIPT(KncokbackAbilityScript);

KncokbackAbilityScript::KncokbackAbilityScript(const Volt::Entity& aEntity) : ScriptBase(aEntity)
{}

void KncokbackAbilityScript::OnAwake()
{
	const auto& data = AbilityData::KnockbackData;

	myTimer = 0;
	auto& mesh = myEntity.AddComponent<Volt::MeshComponent>().handle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Meshes/Primitives/Cube.vtmesh");
	myEntity.GetComponent<Volt::TagComponent>().tag = "Knockback Hitbox";
	auto& transform = myEntity.GetComponent<Volt::TransformComponent>();

	transform.position += myEntity.GetForward() * data->size.z * 0.5f;
	transform.scale = { data->size.x * 0.01f, data->size.y * 0.01f, data->size.z * 0.01f };
}

void KncokbackAbilityScript::OnStart()
{}

void KncokbackAbilityScript::OnUpdate(float aDeltaTime)
{
	myTimer += aDeltaTime;
	if (myTimer >= 0.25)
	{
		myEntity.GetScene()->RemoveEntity(myEntity);
		return;
	}
}

void KncokbackAbilityScript::OnStop()
{}

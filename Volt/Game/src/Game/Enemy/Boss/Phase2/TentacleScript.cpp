#include "TentacleScript.h"
#include "Game/Abilities/Enemy/LineSlamAbility.h"
#include <Volt/Components/Components.h>
#include "Volt/Asset/AssetManager.h"

VT_REGISTER_SCRIPT(TentacleScript)

TentacleScript::TentacleScript(const Volt::Entity& aEntity) : ScriptBase(aEntity)
{}

TentacleScript::~TentacleScript()
{}

void TentacleScript::OnAwake()
{
	mySM = CreateRef<TentacleSM>(myEntity);
	mySM->CreateStates();
	myLineSlam = CreateRef<LineSlamAbility>(myEntity);
	myEntity.AddComponent<Volt::MeshComponent>().handle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Meshes/Primitives/Cube.vtmesh");
}

void TentacleScript::OnStart()
{}

void TentacleScript::OnUpdate(float deltaTime)
{
	myLineSlam->Update(deltaTime);
	mySM->Update(deltaTime);

}

void TentacleScript::OnStop()
{}

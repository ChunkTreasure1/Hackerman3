#include "PlayerDeathState.h"
#include "Volt/Animation/AnimationManager.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Asset/Animation/AnimatedCharacter.h"
#include "Volt/Components/Components.h"

PlayerDeathState::PlayerDeathState(const Volt::Entity& aEntity)
	:StateBase(aEntity)
{
}

void PlayerDeathState::OnEnter()
{
	auto& animComp = myEntity.GetComponent<Volt::AnimatedCharacterComponent>();

	animComp.currentAnimation = 8;
	animComp.currentStartTime = Volt::AnimationManager::globalClock;
	animComp.isLooping = false;
}

void PlayerDeathState::OnUpdate(const float& deltaTime)
{
}

void PlayerDeathState::OnExit()
{
	auto& animComp = myEntity.GetComponent<Volt::AnimatedCharacterComponent>();

	animComp.isLooping = true;
}

void PlayerDeathState::OnReset()
{
}

void PlayerDeathState::OnFixedUpdate()
{
}

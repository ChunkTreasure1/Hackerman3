#include "PlayerPrimaryState.h"
#include "Volt/Animation/AnimationManager.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Asset/Animation/AnimatedCharacter.h"
#include "Volt/Components/Components.h"

PlayerPrimaryState::PlayerPrimaryState(const Volt::Entity& aEntity)
	: StateBase(aEntity)
{
}

void PlayerPrimaryState::OnEnter()
{
	auto& animComp = myEntity.GetComponent<Volt::AnimatedCharacterComponent>();

	animComp.currentAnimation = 4;
	animComp.currentStartTime = Volt::AnimationManager::globalClock;

	myTimeActive = Volt::AssetManager::GetAsset<Volt::AnimatedCharacter>(animComp.animatedCharacter)->GetAnimationDuration(4);
}

void PlayerPrimaryState::OnUpdate(const float& deltaTime)
{
	if (myTimeActive <= 0)
	{
		myTransitions[ePlayerState::IDLE] = true;
	}
	else
	{
		myTimeActive -= deltaTime;
	}
}

void PlayerPrimaryState::OnExit()
{
}

void PlayerPrimaryState::OnReset()
{
}

void PlayerPrimaryState::OnFixedUpdate()
{
}

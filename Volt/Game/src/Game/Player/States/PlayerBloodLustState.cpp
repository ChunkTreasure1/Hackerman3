#include "PlayerBloodLustState.h"
#include "Volt/Animation/AnimationManager.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Asset/Animation/AnimatedCharacter.h"
#include "Volt/Components/Components.h"

PlayerBloodLustState::PlayerBloodLustState(const Volt::Entity& aEntity)
	: StateBase(aEntity)
{
}

void PlayerBloodLustState::OnEnter()
{
	auto& animComp = myEntity.GetComponent<Volt::AnimatedCharacterComponent>();

	animComp.currentAnimation = 3;
	animComp.currentStartTime = Volt::AnimationManager::globalClock;

	myTimeActive = Volt::AssetManager::GetAsset<Volt::AnimatedCharacter>(animComp.animatedCharacter)->GetAnimationDuration(3);
}

void PlayerBloodLustState::OnUpdate(const float& deltaTime)
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

void PlayerBloodLustState::OnExit()
{
}

void PlayerBloodLustState::OnReset()
{
}

void PlayerBloodLustState::OnFixedUpdate()
{
}

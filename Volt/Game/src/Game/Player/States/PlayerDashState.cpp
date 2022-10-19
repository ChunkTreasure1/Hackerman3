#include "PlayerDashState.h"
#include "Volt/Animation/AnimationManager.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Asset/Animation/AnimatedCharacter.h"
#include "Volt/Components/Components.h"

PlayerDashState::PlayerDashState(const Volt::Entity& aEntity)
	: StateBase(aEntity)
{
}

void PlayerDashState::OnEnter()
{
	auto& animComp = myEntity.GetComponent<Volt::AnimatedCharacterComponent>();

	animComp.currentAnimation = 1;
	animComp.currentStartTime = Volt::AnimationManager::globalClock;

	myTimeActive = Volt::AssetManager::GetAsset<Volt::AnimatedCharacter>(animComp.animatedCharacter)->GetAnimationDuration(1);
}

void PlayerDashState::OnUpdate(const float& deltaTime)
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

void PlayerDashState::OnExit()
{
}

void PlayerDashState::OnReset()
{
}

void PlayerDashState::OnFixedUpdate()
{
}

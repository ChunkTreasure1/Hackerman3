#include "PlayerFearState.h"
#include "Volt/Animation/AnimationManager.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Asset/Animation/AnimatedCharacter.h"
#include "Volt/Components/Components.h"

PlayerFearState::PlayerFearState(const Volt::Entity& aEntity)
	: StateBase(aEntity)
{
}

void PlayerFearState::OnEnter()
{
	auto& animComp = myEntity.GetComponent<Volt::AnimatedCharacterComponent>();

	animComp.currentAnimation = 2;
	animComp.currentStartTime = Volt::AnimationManager::globalClock;

	myTimeActive = Volt::AssetManager::GetAsset<Volt::AnimatedCharacter>(animComp.animatedCharacter)->GetAnimationDuration(2);
}

void PlayerFearState::OnUpdate(const float& deltaTime)
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

void PlayerFearState::OnExit()
{
}

void PlayerFearState::OnReset()
{
}

void PlayerFearState::OnFixedUpdate()
{
}

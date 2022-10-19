#include "PlayerArrowState.h"
#include "Volt/Animation/AnimationManager.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Asset/Animation/AnimatedCharacter.h"
#include "Volt/Components/Components.h"

PlayerArrowState::PlayerArrowState(const Volt::Entity& aEntity)
	: StateBase(aEntity)
{
}

void PlayerArrowState::OnEnter()
{
	auto& animComp = myEntity.GetComponent<Volt::AnimatedCharacterComponent>();

	animComp.currentAnimation = 5;
	animComp.currentStartTime = Volt::AnimationManager::globalClock;

	myTimeActive = Volt::AssetManager::GetAsset<Volt::AnimatedCharacter>(animComp.animatedCharacter)->GetAnimationDuration(5);
}

void PlayerArrowState::OnUpdate(const float& deltaTime)
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

void PlayerArrowState::OnExit()
{
}

void PlayerArrowState::OnReset()
{
}

void PlayerArrowState::OnFixedUpdate()
{
}

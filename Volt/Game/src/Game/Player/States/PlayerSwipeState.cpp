#include "PlayerSwipeState.h"
#include "Volt/Animation/AnimationManager.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Asset/Animation/AnimatedCharacter.h"
#include "Volt/Components/Components.h"
#include "Game/Abilities/AbilityDescriptions.hpp"
#include "Volt/Physics/PhysicsActor.h"

PlayerSwipeState::PlayerSwipeState(const Volt::Entity& aEntity)
	: StateBase(aEntity)
{
}

void PlayerSwipeState::OnEnter()
{
	auto& animComp = myEntity.GetComponent<Volt::AnimatedCharacterComponent>();

	animComp.currentAnimation = 0;
	animComp.currentStartTime = Volt::AnimationManager::globalClock;
	
	myTimeActive = Volt::AssetManager::GetAsset<Volt::AnimatedCharacter>(animComp.animatedCharacter)->GetAnimationDuration(0);
}

void PlayerSwipeState::OnUpdate(const float& deltaTime)
{
	if (myTimeActive <= 0)
	{
		myTransitions[ePlayerState::IDLE] = true;
	}
	else
	{
		if (myIsMoving)
		{
			myEntity.SetPosition(myEntity.GetPosition() + myEntity.GetForward() * AbilityData::SwipeData->swipeMoveSpeed * deltaTime);
		}
			
		myTimeActive -= deltaTime;
	}
}



void PlayerSwipeState::OnExit()
{
}

void PlayerSwipeState::OnReset()
{
}

void PlayerSwipeState::OnFixedUpdate()
{
}

void PlayerSwipeState::PlayerIsMoving(bool isMoving)
{
	myIsMoving = isMoving;
}

#include "PlayerWalkState.h"
#include "Volt/Components/Components.h"
#include "Volt/Animation/AnimationManager.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Asset/Animation/AnimatedCharacter.h"
#include "Volt/Log/Log.h"

PlayerWalkState::PlayerWalkState(const Volt::Entity& aEntity)
	: StateBase(aEntity)
{
	myTargetPosition = { 0,0,0 };
}

void PlayerWalkState::OnEnter()
{
	auto& animComp = myEntity.GetComponent<Volt::AnimatedCharacterComponent>();

	animComp.currentAnimation = 7;
	animComp.currentStartTime = Volt::AnimationManager::globalClock;
}

void PlayerWalkState::OnUpdate(const float& deltaTime)
{
	float distToTarget = gem::length(myTargetPosition - myEntity.GetPosition());

	if (distToTarget > 40)
	{
		MoveTowardsTarget(deltaTime);
	}
	else
	{
		myTransitions[ePlayerState::IDLE] = true;
	}

	auto& transform = myEntity.GetComponent<Volt::TransformComponent>();

	gem::mat test = gem::lookAtLH(transform.position, myTargetPosition, { 0,1,0 });
	gem::vec3 rot = 0;
	gem::vec3 dump = 0;
	gem::decompose(test, dump, rot, dump);
	transform.rotation = rot;
	transform.rotation.y *= -1.0f;
}

void PlayerWalkState::OnExit()
{
	myTargetPosition = myEntity.GetPosition();
}

void PlayerWalkState::OnReset()
{
}

void PlayerWalkState::OnFixedUpdate()
{
}

void PlayerWalkState::MoveTowardsTarget(const float& deltaTime)
{
	gem::vec3 dir = gem::normalize(myTargetPosition - myEntity.GetComponent<Volt::TransformComponent>().position);

	myEntity.SetPosition(myEntity.GetPosition() + dir * myEntity.GetComponent<Volt::PlayerComponent>().walkSpeed * deltaTime);
}

void PlayerWalkState::SetTargetPos(gem::vec3 aPos)
{
	myTargetPosition = aPos;
}

void PlayerWalkState::SetViewportValues(gem::vec2 aVPMousePos, gem::vec2 aVPSize)
{
	myViewportMousePos = aVPMousePos;
	myViewportSize = aVPSize;
}

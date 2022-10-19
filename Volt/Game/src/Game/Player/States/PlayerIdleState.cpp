#include "PlayerIdleState.h"
#include "Volt/Log/Log.h"
#include "Volt/Animation/AnimationManager.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Asset/Animation/AnimatedCharacter.h"
#include "Volt/Components/Components.h"

PlayerIdleState::PlayerIdleState(const Volt::Entity& aEntity)
	: StateBase(aEntity)
{
}

void PlayerIdleState::OnEnter()
{
}

void PlayerIdleState::OnUpdate(const float& deltaTime)
{
}

void PlayerIdleState::OnExit()
{
}

void PlayerIdleState::OnReset()
{
}

void PlayerIdleState::OnFixedUpdate()
{
}

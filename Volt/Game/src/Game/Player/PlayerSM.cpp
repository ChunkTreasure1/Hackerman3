#include "PlayerSM.h"

PlayerSM::PlayerSM(const Volt::Entity& aEntity)
	:StateMachineBase(aEntity)
{
}

void PlayerSM::CreateStates()
{
	Ref<StateBase<ePlayerState>> idle = CreateRef<PlayerIdleState>(myEntity);
	Ref<StateBase<ePlayerState>> walk = CreateRef<PlayerWalkState>(myEntity);

	myStates.insert({ ePlayerState::IDLE, idle });
	myStates.insert({ ePlayerState::WALK, walk });

	myStates[ePlayerState::IDLE]->AddTransition(ePlayerState::WALK);
	myStates[ePlayerState::WALK]->AddTransition(ePlayerState::IDLE);

	myActiveState = ePlayerState::IDLE;
}
#include "TentacleSM.h"
#include "Game/Enemy/Boss/Phase2/TentacleStates/TentacleMainState.h"
#include "Game/Enemy/Boss/Phase2/TentacleStates/TentacleAttackState.h"

TentacleSM::TentacleSM(const Volt::Entity& aEntity) : StateMachineBase(aEntity)
{}

void TentacleSM::CreateStates()
{
	myStates.insert({ eTentacleState::MAIN, CreateRef<TentacleMainState>(myEntity) });
	myStates.insert({ eTentacleState::ATTACK, CreateRef<TentacleAttackState>(myEntity) });

	myStates[eTentacleState::MAIN]->AddTransition(eTentacleState::ATTACK);
	myStates[eTentacleState::ATTACK]->AddTransition(eTentacleState::MAIN);

	myActiveState = eTentacleState::MAIN;
	myStates[myActiveState]->OnEnter();
}

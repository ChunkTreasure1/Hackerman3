#include "BruiserEnemySM.h"
#include "Game/Enemy/Bruiser/States/BruiserEnemyAttackState.h"
#include "Game/Enemy/Bruiser/States/BruiserEnemyIdleState.h"
#include "Game/Enemy/Bruiser/States/BruiserEnemyWalkState.h"

BruiserEnemySM::BruiserEnemySM(const Volt::Entity& aEntity) : StateMachineBase(aEntity)
{}

void BruiserEnemySM::CreateStates()
{
	Ref<StateBase<eBruiserEnemyState>> entry = CreateRef<BruiserEnemyIdleState>(myEntity);

	myStates.insert({ eBruiserEnemyState::IDLE, entry });
	entry->AddTransition(eBruiserEnemyState::WALK);

	entry = CreateRef<BruiserEnemyAttackState>(myEntity);
	myStates.insert({ eBruiserEnemyState::ATTACK, entry });
	entry->AddTransition(eBruiserEnemyState::WALK);

	entry = CreateRef<BruiserEnemyWalkState>(myEntity);
	myStates.insert({ eBruiserEnemyState::WALK, entry });
	entry->AddTransition(eBruiserEnemyState::ATTACK);

	myActiveState = eBruiserEnemyState::IDLE;
	myStates[myActiveState]->OnEnter();
}

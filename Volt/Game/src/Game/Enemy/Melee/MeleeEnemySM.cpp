#include "MeleeEnemySM.h"
#include "Game/Enemy/Melee/States/MeleeEnemyAttackState.h"
#include "Game/Enemy/Melee/States/MeleeEnemyIdleState.h"
#include "Game/Enemy/Melee/States/MeleeEnemyWalkState.h"

MeleeEnemySM::MeleeEnemySM(const Volt::Entity& aEntity) : StateMachineBase(aEntity)
{}

void MeleeEnemySM::CreateStates()
{
	Ref<StateBase<eMeleeEnemyState>> entry = CreateRef<MeleeEnemyIdleState>(myEntity);

	myStates.insert({ eMeleeEnemyState::IDLE, entry });
	entry->AddTransition(eMeleeEnemyState::WALK);

	entry = CreateRef<MeleeEnemyAttackState>(myEntity);
	myStates.insert({ eMeleeEnemyState::ATTACK, entry });
	entry->AddTransition(eMeleeEnemyState::WALK);

	entry = CreateRef<MeleeEnemyWalkState>(myEntity);
	myStates.insert({ eMeleeEnemyState::WALK, entry });
	entry->AddTransition(eMeleeEnemyState::ATTACK);

	myActiveState = eMeleeEnemyState::IDLE;
	myStates[myActiveState]->OnEnter();
}

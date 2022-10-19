#include "RangedEnemySM.h"
#include "Game/Enemy/Ranged/States/RangedEnemyAttackState.h"
#include "Game/Enemy/Ranged/States/RangedEnemyIdleState.h"
#include "Game/Enemy/Ranged/States/RangedEnemyWalkState.h"
#include "Game/Enemy/Ranged/States/RangedEnemyFleeState.h"

RangedEnemySM::RangedEnemySM(const Volt::Entity& aEntity) : StateMachineBase(aEntity)
{}

void RangedEnemySM::CreateStates()
{
	Ref<StateBase<eRangedEnemyState>> entry = CreateRef<RangedEnemyIdleState>(myEntity);

	myStates.insert({ eRangedEnemyState::IDLE, entry });
	entry->AddTransition(eRangedEnemyState::WALK);

	entry = CreateRef<RangedEnemyAttackState>(myEntity);
	myStates.insert({ eRangedEnemyState::ATTACK, entry });

	entry = CreateRef<RangedEnemyWalkState>(myEntity);
	myStates.insert({ eRangedEnemyState::WALK, entry });
	entry->AddTransition(eRangedEnemyState::ATTACK);

	entry = CreateRef<RangedEnemyFleeState>(myEntity);
	myStates.insert({ eRangedEnemyState::FLEE, entry });
	entry->AddTransition(eRangedEnemyState::ATTACK);

	myActiveState = eRangedEnemyState::IDLE;
}

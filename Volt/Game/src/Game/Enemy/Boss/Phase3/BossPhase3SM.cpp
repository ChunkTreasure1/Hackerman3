#include "BossPhase3SM.h"
#include "Game/Enemy/Boss/Phase3/States/BossPhase3KnockbackState.h"
#include "Game/Enemy/Boss/Phase3/States/BossPhase3LineSlamState.h"
#include "Game/Enemy/Boss/Phase3/States/BossPhase3GasAttackState.h"
#include "Game/Enemy/Boss/Phase3/States/BossPhase3MainState.h"

BossPhase3SM::BossPhase3SM(const Volt::Entity& aEntity) : StateMachineBase(aEntity)
{}

void BossPhase3SM::CreateStates()
{
	myStates.insert({ eBossPhase3State::GASATTACK, CreateRef<BossPhase3GasAttackState>(myEntity) });
	myStates.insert({ eBossPhase3State::KNOCKBACK, CreateRef<BossPhase3KnockbackState>(myEntity) });
	myStates.insert({ eBossPhase3State::LINESLAM, CreateRef<BossPhase3LineSlamState>(myEntity) });
	myStates.insert({ eBossPhase3State::MAIN, CreateRef<BossPhase3MainState>(myEntity) });

	myStates[eBossPhase3State::GASATTACK]->AddTransition(eBossPhase3State::MAIN);
	myStates[eBossPhase3State::KNOCKBACK]->AddTransition(eBossPhase3State::MAIN);
	myStates[eBossPhase3State::LINESLAM]->AddTransition(eBossPhase3State::MAIN);
	myStates[eBossPhase3State::MAIN]->AddTransition(eBossPhase3State::GASATTACK);
	myStates[eBossPhase3State::MAIN]->AddTransition(eBossPhase3State::KNOCKBACK);
	myStates[eBossPhase3State::MAIN]->AddTransition(eBossPhase3State::LINESLAM);

	myActiveState = eBossPhase3State::MAIN;
}

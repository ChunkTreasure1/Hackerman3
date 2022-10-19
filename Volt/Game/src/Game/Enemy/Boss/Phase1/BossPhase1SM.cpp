#include "BossPhase1SM.h"
#include "Game/Enemy/Boss/Phase1/States/BossPhase1KnockbackState.h"
#include "Game/Enemy/Boss/Phase1/States/BossPhase1LineSlamState.h"
#include "Game/Enemy/Boss/Phase1/States/BossPhase1MainState.h"

BossPhase1SM::BossPhase1SM(const Volt::Entity& aEntity) : StateMachineBase(aEntity)
{}

void BossPhase1SM::CreateStates()
{
	myStates.insert({ eBossPhase1State::KNOCKBACK, CreateRef<BossPhase1KnockbackState>(myEntity) });
	myStates.insert({ eBossPhase1State::LINESLAM, CreateRef<BossPhase1LineSlamState>(myEntity) });
	myStates.insert({ eBossPhase1State::MAIN, CreateRef<BossPhase1MainState>(myEntity) });

	myStates[eBossPhase1State::KNOCKBACK]->AddTransition(eBossPhase1State::MAIN);
	myStates[eBossPhase1State::LINESLAM]->AddTransition(eBossPhase1State::MAIN);
	myStates[eBossPhase1State::MAIN]->AddTransition(eBossPhase1State::LINESLAM);
	myStates[eBossPhase1State::MAIN]->AddTransition(eBossPhase1State::KNOCKBACK);

	myActiveState = eBossPhase1State::MAIN;
}

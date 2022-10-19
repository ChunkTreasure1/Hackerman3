#include "BossSM.h"
#include "Game/Enemy/Boss/Phase1/BossPhase1State.h"
#include "Game/Enemy/Boss/Phase2/BossPhase2State.h"
#include "Game/Enemy/Boss/Phase3/BossPhase3State.h"
// Phase State Machine 
BossSM::BossSM(const Volt::Entity& aEntity) : StateMachineBase(aEntity)
{}

void BossSM::CreateStates()
{
	myStates.insert({ eBossPhaseState::PHASE1, CreateRef<BossPhase1State>(myEntity) });
	myStates.insert({ eBossPhaseState::PHASE2, CreateRef<BossPhase2State>(myEntity) });
	myStates.insert({ eBossPhaseState::PHASE3, CreateRef<BossPhase3State>(myEntity) });

	myStates[eBossPhaseState::PHASE1]->AddTransition(eBossPhaseState::PHASE2);
	myStates[eBossPhaseState::PHASE2]->AddTransition(eBossPhaseState::PHASE3);

	myActiveState = eBossPhaseState::PHASE1;
	myStates[myActiveState]->OnEnter();
}


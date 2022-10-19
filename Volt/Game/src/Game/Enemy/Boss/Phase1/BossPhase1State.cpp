#include "BossPhase1State.h"
#include <Volt/Components/Components.h>
#include "Game/Enemy/Boss/BossScript.h"
#include <Volt/Log/Log.h>
BossPhase1State::BossPhase1State(const Volt::Entity& aEntity) : StateBase(aEntity)
{
	mySM = CreateRef<BossPhase1SM>(myEntity);
	mySM->CreateStates();
	mySM->GetStates()[mySM->GetActiveState()]->OnEnter();
}

void BossPhase1State::OnExit()
{
	mySM->GetStates()[mySM->GetActiveState()]->OnExit();
}

void BossPhase1State::OnEnter()
{
	VT_CORE_INFO("Boss Phase 1 entered");
}

void BossPhase1State::OnReset()
{}

void BossPhase1State::OnUpdate(const float& deltaTime)
{
	mySM->Update(deltaTime);
	float cHP = myEntity.GetComponent<Volt::HealthComponent>().health;
	float sHP = myEntity.GetScript<BossScript>("BossScript")->GetStartHP();
	float res = (float)cHP / (float)sHP;
	if (res <= myEntity.GetComponent<Volt::BossComponent>().phase2p)
		SetTransition(eBossPhaseState::PHASE2);
}

void BossPhase1State::OnFixedUpdate()
{}

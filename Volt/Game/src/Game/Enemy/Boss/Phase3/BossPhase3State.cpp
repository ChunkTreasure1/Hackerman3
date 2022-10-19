#include "BossPhase3State.h"
#include <Volt/Log/Log.h>
#include <Volt/Components/Components.h>

BossPhase3State::BossPhase3State(const Volt::Entity& aEntity) : StateBase(aEntity)
{
	mySM = CreateRef<BossPhase3SM>(myEntity);
	mySM->CreateStates();
}

void BossPhase3State::OnExit()
{}

void BossPhase3State::OnEnter()
{
	mySM->GetStates()[mySM->GetActiveState()]->OnEnter();
	VT_CORE_INFO("Boss Phase 3 entered");
}

void BossPhase3State::OnReset()
{}

void BossPhase3State::OnUpdate(const float& deltaTime)

{
	mySM->Update(deltaTime);
	if (myEntity.GetComponent<Volt::HealthComponent>().health <= 0)
		myEntity.GetScene()->RemoveEntity(myEntity);
}

void BossPhase3State::OnFixedUpdate()
{}

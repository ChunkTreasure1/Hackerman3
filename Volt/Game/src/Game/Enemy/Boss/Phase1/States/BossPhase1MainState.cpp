#include "BossPhase1MainState.h"
#include "Game/Enemy/Boss/BossScript.h"
#include <Volt/Components/Components.h>
#include "Game/Enemy/Boss/AbilityHandler.h"

BossPhase1MainState::BossPhase1MainState(const Volt::Entity& aEntity) : StateBase(aEntity)
{}

void BossPhase1MainState::OnExit()
{
	myKnockbackEntity.GetScene()->RemoveEntity(myKnockbackEntity);
}

void BossPhase1MainState::OnEnter()
{
	myKnockbackEntity = myEntity.GetScene()->CreateEntity();
	myKnockbackEntity.GetComponent<Volt::TagComponent>().tag = "knockback Entity";
	myKnockbackTimer = myEntity.GetComponent<Volt::BossComponent>().knockbackProc;
	gem::vec3 target = { 0.0f, 0.0f, 0.0f };
	bool pFound = false;
	myEntity.GetScene()->GetRegistry().ForEach<Volt::PlayerComponent, Volt::TransformComponent>([&target, &pFound](Wire::EntityId id, const Volt::PlayerComponent& playerComponent, const Volt::TransformComponent& transformComponent)
		{
			// NEEDS TO BE SET TO GET WORLD POS
			target = transformComponent.position;
			pFound = true;
		});
	if (!pFound)
		return;

	myKnockbackEntity.SetWorldPosition(target);
}

void BossPhase1MainState::OnReset()
{}

void BossPhase1MainState::OnUpdate(const float& deltaTime)
{
	// Find player pos
	gem::vec3 target = { 0.0f, 0.0f, 0.0f };
	bool pFound = false;
	myEntity.GetScene()->GetRegistry().ForEach<Volt::PlayerComponent, Volt::TransformComponent>([&target, &pFound](Wire::EntityId id, const Volt::PlayerComponent& playerComponent, const Volt::TransformComponent& transformComponent)
		{
			// WORLD POS 
			target = transformComponent.position;
			pFound = true;
		});
	if (!pFound)
		return;
	auto abilityHandler = myEntity.GetScript<BossScript>("BossScript")->GetAbilityHandler();
	auto trans = abilityHandler->GetEntity();//.GetComponent<Volt::TransformComponent>();
	gem::mat test = gem::lookAt(trans.GetWorldPosition(), target, { 0,1,0 });
	gem::vec3 rot = 0;
	gem::vec3 dump = 0;
	gem::decompose(test, dump, rot, dump);
	rot.y *= -1;
	trans.SetRotation(rot);
	//trans.GetComponent<Volt::TransformComponent>().rotation = rot;
	//trans.GetComponent<Volt::TransformComponent>().rotation.y *= -1.0f;

	if (abilityHandler->IsCastable(eBossAbility::LINESLAM))
	{
		SetTransition(eBossPhase1State::LINESLAM);
		return;
	}

	myKnockbackTimer -= deltaTime;
	if (gem::distance(myKnockbackEntity.GetWorldPosition(), target) > myEntity.GetComponent<Volt::BossComponent>().knockbackResetDistance)
	{
		myKnockbackEntity.GetComponent<Volt::TransformComponent>().position = target;
		myKnockbackTimer = myEntity.GetComponent<Volt::BossComponent>().knockbackProc;
		myKnockbackEntity.SetWorldPosition(target);
		return;
	}
	if (myKnockbackTimer <= 0 && abilityHandler->IsCastable(eBossAbility::KNOCKBACK))
	{
		SetTransition(eBossPhase1State::KNOCKBACK);
	}
}

void BossPhase1MainState::OnFixedUpdate()
{}

#include "BossPhase1KnockbackState.h"
#include "Game/Enemy/Boss/BossScript.h"
#include <Volt/Components/Components.h>

BossPhase1KnockbackState::BossPhase1KnockbackState(const Volt::Entity& aEntity) : StateBase(aEntity)
{}

void BossPhase1KnockbackState::OnExit()
{}

void BossPhase1KnockbackState::OnEnter()
{
	gem::vec3 target = { 0.0f, 0.0f, 0.0f };
	bool pFound = false;
	myEntity.GetScene()->GetRegistry().ForEach<Volt::PlayerComponent, Volt::TransformComponent>([&target, &pFound](Wire::EntityId id, const Volt::PlayerComponent& playerComponent, const Volt::TransformComponent& transformComponent)
		{
			target = transformComponent.position;
			pFound = true;
		});
	if (!pFound)
		SetTransition(eBossPhase1State::MAIN);
	auto abilityHandler = myEntity.GetScript<BossScript>("BossScript")->GetAbilityHandler();
	auto abilityEntTransform = abilityHandler->GetEntity();//.GetComponent<Volt::TransformComponent>();
	gem::mat test = gem::lookAt(abilityEntTransform.GetWorldPosition(), target, { 0,1,0 });
	gem::vec3 rot = 0;
	gem::vec3 dump = 0;
	gem::decompose(test, dump, rot, dump);
	abilityEntTransform.GetComponent<Volt::TransformComponent>().rotation = rot;
	abilityEntTransform.GetComponent<Volt::TransformComponent>().rotation.y *= -1.0f;

	abilityHandler->Cast(eBossAbility::KNOCKBACK);
	SetTransition(eBossPhase1State::MAIN);
}

void BossPhase1KnockbackState::OnReset()
{}

void BossPhase1KnockbackState::OnUpdate(const float& deltaTime)
{}

void BossPhase1KnockbackState::OnFixedUpdate()
{}

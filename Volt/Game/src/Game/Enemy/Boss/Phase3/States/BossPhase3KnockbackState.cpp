#include "BossPhase3KnockbackState.h"
#include "Game/Enemy/Boss/BossScript.h"
#include <Volt/Components/Components.h>

BossPhase3KnockbackState::BossPhase3KnockbackState(const Volt::Entity& aEntity) : StateBase(aEntity)
{}

void BossPhase3KnockbackState::OnExit()
{}

void BossPhase3KnockbackState::OnEnter()
{
	gem::vec3 target = { 0.0f, 0.0f, 0.0f };
	bool pFound = false;
	myEntity.GetScene()->GetRegistry().ForEach<Volt::PlayerComponent, Volt::TransformComponent>([&target, &pFound](Wire::EntityId id, const Volt::PlayerComponent& playerComponent, const Volt::TransformComponent& transformComponent)
		{
			target = transformComponent.position;
			pFound = true;
		});
	if (!pFound)
		SetTransition(eBossPhase3State::MAIN);
	auto abilityHandler = myEntity.GetScript<BossScript>("BossScript")->GetAbilityHandler();
	auto abilityEntTransform = abilityHandler->GetEntity();//.GetComponent<Volt::TransformComponent>();
	gem::mat test = gem::lookAt(abilityEntTransform.GetWorldPosition(), target, { 0,1,0 });
	gem::vec3 rot = 0;
	gem::vec3 dump = 0;
	gem::decompose(test, dump, rot, dump);
	abilityEntTransform.GetComponent<Volt::TransformComponent>().rotation = rot;
	abilityEntTransform.GetComponent<Volt::TransformComponent>().rotation.y *= -1.0f;

	abilityHandler->Cast(eBossAbility::KNOCKBACK);
	SetTransition(eBossPhase3State::MAIN);
}

void BossPhase3KnockbackState::OnReset()
{}

void BossPhase3KnockbackState::OnUpdate(const float& deltaTime)
{}

void BossPhase3KnockbackState::OnFixedUpdate()
{}

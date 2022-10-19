#include "RangedEnemyAttackState.h"
#include <Volt/Components/Components.h>
#include <Game/Abilities/AbilityDescriptions.hpp>

RangedEnemyAttackState::RangedEnemyAttackState(const Volt::Entity& aEntity) : StateBase(aEntity)
{
	myRangedAbility = CreateRef<RangedAbility>(myEntity);

}

void RangedEnemyAttackState::OnExit()
{}

void RangedEnemyAttackState::OnEnter()
{
	myRangedAbility->SetCooldownTimer(0.5f);
}

void RangedEnemyAttackState::OnReset()
{}

void RangedEnemyAttackState::OnUpdate(const float& deltaTime)
{
	myFleeDistance = myEntity.GetComponent<Volt::RangedEnemyComponent>().fleeDistance;
	auto& transform = myEntity.GetComponent<Volt::TransformComponent>();
	gem::vec3 target = { 0.0f, 0.0f, 0.0f };
	myEntity.GetScene()->GetRegistry().ForEach<Volt::PlayerComponent, Volt::TransformComponent>([&target](Wire::EntityId id, const Volt::PlayerComponent& playerComponent, const Volt::TransformComponent& transformComponent)
		{
			target = transformComponent.position;

		});
	if (gem::distance(transform.position, target) >= AbilityData::RangedAttackData->range)
		myTransitions[eRangedEnemyState::WALK] = true;

	if (gem::distance(transform.position, target) <= AbilityData::RangedAttackData->range * myFleeDistance)
		myTransitions[eRangedEnemyState::FLEE] = true;

	gem::mat test = gem::lookAt(transform.position, target, { 0,1,0 });
	gem::vec3 rot = 0;
	gem::vec3 dump = 0;
	gem::decompose(test, dump, rot, dump);
	transform.rotation = rot;
	transform.rotation.y *= -1.0f;
	myRangedAbility->Update(deltaTime);
	myRangedAbility->Cast();
}

void RangedEnemyAttackState::OnFixedUpdate()
{}

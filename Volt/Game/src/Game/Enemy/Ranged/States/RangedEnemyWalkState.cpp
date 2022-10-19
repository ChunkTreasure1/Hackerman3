#include "RangedEnemyWalkState.h"
#include <Volt/Components/Components.h>
#include <GEM/gem.h>
#include <Game/Abilities/AbilityDescriptions.hpp>

RangedEnemyWalkState::RangedEnemyWalkState(const Volt::Entity& aEntity) : StateBase(aEntity)
{
	myMoveSpeed = 30;
}

void RangedEnemyWalkState::OnExit()
{}

void RangedEnemyWalkState::OnEnter()
{}

void RangedEnemyWalkState::OnReset()
{}

void RangedEnemyWalkState::OnUpdate(const float& deltaTime)
{
	myMoveSpeed = myEntity.GetComponent<Volt::RangedEnemyComponent>().moveSpeed;
	myEnterAttack = myEntity.GetComponent<Volt::RangedEnemyComponent>().enterAttack;

	auto& transform = myEntity.GetComponent<Volt::TransformComponent>();
	gem::vec3 target = { 0.0f, 0.0f, 0.0f };
	myEntity.GetScene()->GetRegistry().ForEach<Volt::PlayerComponent, Volt::TransformComponent>([&target](Wire::EntityId id, const Volt::PlayerComponent& playerComponent, const Volt::TransformComponent& transformComponent)
		{
			target = transformComponent.position;

		});
	gem::mat test = gem::lookAt(transform.position, target, { 0,1,0 });
	gem::vec3 rot = 0;
	gem::vec3 dump = 0;
	gem::decompose(test, dump, rot, dump);
	transform.rotation = rot;
	transform.rotation.y *= -1.0f;

	//transform.position += myEntity.GetWorldForward() * myMoveSpeed * deltaTime;
	gem::vec3 dir = gem::normalize(target - myEntity.GetComponent<Volt::TransformComponent>().position);
	myEntity.GetComponent<Volt::TransformComponent>().position += dir * myEntity.GetComponent<Volt::RangedEnemyComponent>().moveSpeed * deltaTime;

	if (gem::distance(transform.position, target) <= AbilityData::RangedAttackData->range * myEnterAttack)
		myTransitions[eRangedEnemyState::ATTACK] = true;
}

void RangedEnemyWalkState::OnFixedUpdate()
{}

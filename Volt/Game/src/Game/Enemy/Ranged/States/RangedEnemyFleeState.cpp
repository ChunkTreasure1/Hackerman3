#include "RangedEnemyFleeState.h"
#include <Volt/Components/Components.h>
#include <GEM/gem.h>
#include <Game/Abilities/AbilityDescriptions.hpp>

RangedEnemyFleeState::RangedEnemyFleeState(const Volt::Entity& aEntity) : StateBase(aEntity)
{
	myMoveSpeed = 30;
}

void RangedEnemyFleeState::OnExit()
{}

void RangedEnemyFleeState::OnEnter()
{}

void RangedEnemyFleeState::OnReset()
{}

void RangedEnemyFleeState::OnUpdate(const float& deltaTime)
{
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
	//transform.rotation = rot;
	//transform.rotation.y *= -1.0f;
	rot.y *= -1;
	myEntity.SetRotation(rot);
	//transform.position += myEntity.GetWorldForward() * -myMoveSpeed * deltaTime;
	gem::vec3 dir = gem::normalize(target - myEntity.GetComponent<Volt::TransformComponent>().position);
	myEntity.SetWorldPosition(myEntity.GetComponent<Volt::TransformComponent>().position - dir * myEntity.GetComponent<Volt::RangedEnemyComponent>().moveSpeed * deltaTime);

	if (gem::distance(transform.position, target) >= AbilityData::RangedAttackData->range * myEntity.GetComponent<Volt::RangedEnemyComponent>().enterAttack)
		myTransitions[eRangedEnemyState::ATTACK] = true;
}

void RangedEnemyFleeState::OnFixedUpdate()
{}

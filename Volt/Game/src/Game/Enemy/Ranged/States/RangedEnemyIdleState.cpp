#include "RangedEnemyIdleState.h"
#include <Volt/Components/Components.h>

RangedEnemyIdleState::RangedEnemyIdleState(const Volt::Entity& aEntity) : StateBase(aEntity)
{}

void RangedEnemyIdleState::OnExit()
{}

void RangedEnemyIdleState::OnEnter()
{}

void RangedEnemyIdleState::OnReset()
{}

void RangedEnemyIdleState::OnUpdate(const float& deltaTime)
{
	gem::vec3 target = { 0.0f, 0.0f, 0.0f };
	bool pFound = false;
	myEntity.GetScene()->GetRegistry().ForEach<Volt::PlayerComponent, Volt::TransformComponent>([&target, &pFound](Wire::EntityId id, const Volt::PlayerComponent& playerComponent, const Volt::TransformComponent& transformComponent)
		{
			target = transformComponent.position;
			pFound = true;
		});
	if (!pFound)
		return;
	if (gem::distance(myEntity.GetWorldPosition(), target) <= myEntity.GetComponent<Volt::RangedEnemyComponent>().activationDistance)
		SetTransition(eRangedEnemyState::WALK);
}

void RangedEnemyIdleState::OnFixedUpdate()
{}

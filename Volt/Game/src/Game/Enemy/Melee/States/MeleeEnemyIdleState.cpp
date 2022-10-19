#include "MeleeEnemyIdleState.h"
#include <Volt/Components/Components.h>
#include <Volt/Animation/AnimationManager.h>

MeleeEnemyIdleState::MeleeEnemyIdleState(const Volt::Entity& aEntity) : StateBase(aEntity)
{}

void MeleeEnemyIdleState::OnExit()
{}

void MeleeEnemyIdleState::OnEnter()
{
	auto& ac = myEntity.GetComponent<Volt::AnimatedCharacterComponent>();
	ac.currentAnimation = 4;
	ac.isLooping = true;	
	myEntity.GetComponent<Volt::AnimatedCharacterComponent>().currentStartTime = Volt::AnimationManager::globalClock;
}

void MeleeEnemyIdleState::OnReset()
{}

void MeleeEnemyIdleState::OnUpdate(const float& deltaTime)
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
	if (gem::distance(myEntity.GetWorldPosition(), target) <= myEntity.GetComponent<Volt::MeleeEnemyComponent>().activationDistance)
		SetTransition(eMeleeEnemyState::WALK);
}

void MeleeEnemyIdleState::OnFixedUpdate()
{}

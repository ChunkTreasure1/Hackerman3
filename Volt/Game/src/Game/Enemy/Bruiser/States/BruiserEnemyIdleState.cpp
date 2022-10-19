#include "BruiserEnemyIdleState.h"
#include <Volt/Components/Components.h>
#include <Volt/Animation/AnimationManager.h>

BruiserEnemyIdleState::BruiserEnemyIdleState(const Volt::Entity& aEntity) : StateBase(aEntity)
{}

void BruiserEnemyIdleState::OnExit()
{}

void BruiserEnemyIdleState::OnEnter()
{
	auto& ac = myEntity.GetComponent<Volt::AnimatedCharacterComponent>();
	ac.currentAnimation = 0;
	ac.isLooping = true;
	myEntity.GetComponent<Volt::AnimatedCharacterComponent>().currentStartTime = Volt::AnimationManager::globalClock;

}

void BruiserEnemyIdleState::OnReset()
{}

void BruiserEnemyIdleState::OnUpdate(const float& deltaTime)
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
	if (gem::distance(myEntity.GetWorldPosition(), target) <= myEntity.GetComponent<Volt::BruiserEnemyComponent>().activationDistance)
		SetTransition(eBruiserEnemyState::WALK);
}

void BruiserEnemyIdleState::OnFixedUpdate()
{}

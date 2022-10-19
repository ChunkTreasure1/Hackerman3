#include "BruiserEnemyWalkState.h"
#include <Volt/Components/Components.h>
#include <GEM/gem.h>
#include <Game/Abilities/AbilityDescriptions.hpp>
#include <Volt/Animation/AnimationManager.h>

BruiserEnemyWalkState::BruiserEnemyWalkState(const Volt::Entity& aEntity) : StateBase(aEntity)
{
	myMoveSpeed = 10;
}

void BruiserEnemyWalkState::OnExit()
{}

void BruiserEnemyWalkState::OnEnter()
{

	auto& ac = myEntity.GetComponent<Volt::AnimatedCharacterComponent>();
	ac.currentAnimation = 1;
	ac.isLooping = true;
	myEntity.GetComponent<Volt::AnimatedCharacterComponent>().currentStartTime = Volt::AnimationManager::globalClock;
}

void BruiserEnemyWalkState::OnReset()
{}

void BruiserEnemyWalkState::OnUpdate(const float& deltaTime)
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

	rot.y *= -1;
	myEntity.SetRotation(rot);
	/*transform.rotation = rot;
	transform.rotation.y *= -1.0f;*/
	
	//transform.position += myEntity.GetWorldForward() * myMoveSpeed * deltaTime;
	gem::vec3 dir = gem::normalize(target - myEntity.GetComponent<Volt::TransformComponent>().position);
	myEntity.SetWorldPosition(myEntity.GetComponent<Volt::TransformComponent>().position + dir * myEntity.GetComponent<Volt::BruiserEnemyComponent>().moveSpeed * deltaTime);
	if (gem::distance(transform.position, target) <= AbilityData::StunAttackData->size.z * myEntity.GetComponent<Volt::BruiserEnemyComponent>().attackP)
		myTransitions[eBruiserEnemyState::ATTACK] = true;
}

void BruiserEnemyWalkState::OnFixedUpdate()
{}

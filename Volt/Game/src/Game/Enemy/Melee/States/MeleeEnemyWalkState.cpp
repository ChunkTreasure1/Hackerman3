#include "MeleeEnemyWalkState.h"
#include <Volt/Components/Components.h>
#include <GEM/gem.h>
#include <Game/Abilities/AbilityDescriptions.hpp>
#include <Volt/Asset/AssetManager.h>
MeleeEnemyWalkState::MeleeEnemyWalkState(const Volt::Entity& aEntity) : StateBase(aEntity)
{
	myMoveSpeed = myEntity.GetComponent<Volt::MeleeEnemyComponent>().moveSpeed;
}

void MeleeEnemyWalkState::OnExit()
{
	// WALK ANIM
	//myEntity.GetComponent<Volt::AnimatedCharacterComponent>().currentAnimation = 2
}

void MeleeEnemyWalkState::OnEnter()
{
	auto& ac = myEntity.GetComponent<Volt::AnimatedCharacterComponent>();
	ac.currentAnimation = 0;
	ac.isLooping = true;
}

void MeleeEnemyWalkState::OnReset()
{

}

void MeleeEnemyWalkState::OnUpdate(const float& deltaTime)
{
	gem::vec3 target = { 0.0f, 0.0f, 0.0f };
	myEntity.GetScene()->GetRegistry().ForEach<Volt::PlayerComponent, Volt::TransformComponent>([&target](Wire::EntityId id, const Volt::PlayerComponent& playerComponent, const Volt::TransformComponent& transformComponent)
		{
			target = transformComponent.position;

		});
	myMoveSpeed = myEntity.GetComponent<Volt::MeleeEnemyComponent>().moveSpeed;
	myRangeP = myEntity.GetComponent<Volt::MeleeEnemyComponent>().attackP;
	auto& transform = myEntity.GetComponent<Volt::TransformComponent>();

	gem::mat test = gem::lookAt(transform.position, target, { 0,1,0 });
	gem::vec3 rot = 0;
	gem::vec3 dump = 0;
	gem::decompose(test, dump, rot, dump);
	/*transform.rotation = rot;
	transform.rotation.y *= -1.0f;*/
	rot.y *= -1;
	myEntity.SetRotation(rot);
	//transform.position += myEntity.GetWorldForward() * myMoveSpeed * deltaTime;
	gem::vec3 dir = gem::normalize(target - myEntity.GetComponent<Volt::TransformComponent>().position);
	myEntity.SetPosition(myEntity.GetComponent<Volt::TransformComponent>().position + dir * myEntity.GetComponent<Volt::MeleeEnemyComponent>().moveSpeed * deltaTime);

	if (gem::distance(transform.position, target) <= AbilityData::CloseAttackData->size.z * myRangeP)
		myTransitions[eMeleeEnemyState::ATTACK] = true;
}

void MeleeEnemyWalkState::OnFixedUpdate()
{}

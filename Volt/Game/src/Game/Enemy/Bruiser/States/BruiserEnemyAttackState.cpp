#include "BruiserEnemyAttackState.h"
#include <Volt/Components/Components.h>
#include <Game/Abilities/AbilityDescriptions.hpp>
#include <Volt/Animation/AnimationManager.h>	
BruiserEnemyAttackState::BruiserEnemyAttackState(const Volt::Entity& aEntity) : StateBase(aEntity)
{
	myBruiserAbility = CreateRef<StunAbility>(myEntity);

}

void BruiserEnemyAttackState::OnExit()
{}

void BruiserEnemyAttackState::OnEnter()
{
	auto& ac = myEntity.GetComponent<Volt::AnimatedCharacterComponent>().isLooping = false;
	myBruiserAbility->SetCooldownTimer(0.5f);
}

void BruiserEnemyAttackState::OnReset()
{}

void BruiserEnemyAttackState::OnUpdate(const float& deltaTime)
{
	auto& transform = myEntity.GetComponent<Volt::TransformComponent>();
	gem::vec3 target = { 0.0f, 0.0f, 0.0f };
	myEntity.GetScene()->GetRegistry().ForEach<Volt::PlayerComponent, Volt::TransformComponent>([&target](Wire::EntityId id, const Volt::PlayerComponent& playerComponent, const Volt::TransformComponent& transformComponent)
		{
			target = transformComponent.position;

		});
	myBruiserAbility->Update(deltaTime);
	if (gem::distance(transform.position, target) >= AbilityData::StunAttackData->size.z && myBruiserAbility->IsCastable())
	{
		myTransitions[eBruiserEnemyState::WALK] = true;
		return;
	}
	gem::mat test = gem::lookAt(transform.position, target, { 0,1,0 });
	gem::vec3 rot = 0;
	gem::vec3 dump = 0;
	gem::decompose(test, dump, rot, dump);
	transform.rotation = rot;
	transform.rotation.y *= -1.0f;
	if (myBruiserAbility->IsCastable())
	{
		myIsCasting = true;
		auto& ac = myEntity.GetComponent<Volt::AnimatedCharacterComponent>();
		ac.currentAnimation = 2;
		ac.isLooping = false;
		myEntity.GetComponent<Volt::AnimatedCharacterComponent>().currentStartTime = Volt::AnimationManager::globalClock;

		myBruiserAbility->CastAfterTime(0.75f);
	}
}

void BruiserEnemyAttackState::OnFixedUpdate()
{}

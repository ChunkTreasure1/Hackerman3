#include "MeleeEnemyAttackState.h"
#include <Volt/Components/Components.h>
#include <Game/Abilities/AbilityDescriptions.hpp>
#include <Volt/Animation/AnimationManager.h>
#include <Volt/Asset/AssetManager.h>
#include <Volt/Asset/Animation/AnimatedCharacter.h>

MeleeEnemyAttackState::MeleeEnemyAttackState(const Volt::Entity& aEntity) : StateBase(aEntity)
{
	myMeleeAbility = CreateRef<MeleeAbility>(myEntity);

}

void MeleeEnemyAttackState::OnExit()
{}

void MeleeEnemyAttackState::OnEnter()
{
	auto& ac = myEntity.GetComponent<Volt::AnimatedCharacterComponent>();
	ac.isLooping = true;
	myMeleeAbility->SetCooldownTimer(0.25f);
}

void MeleeEnemyAttackState::OnReset()
{}

void MeleeEnemyAttackState::OnUpdate(const float& deltaTime)
{
	myAnimationTime += deltaTime;
	auto& transform = myEntity.GetComponent<Volt::TransformComponent>();
	gem::vec3 target = { 0.0f, 0.0f, 0.0f };

	myEntity.GetScene()->GetRegistry().ForEach<Volt::PlayerComponent, Volt::TransformComponent>([&target](Wire::EntityId id, const Volt::PlayerComponent& playerComponent, const Volt::TransformComponent& transformComponent)
		{
			target = transformComponent.position;

		});

	if (!myAnimationStarted)
	{
		gem::mat test = gem::lookAt(transform.position, target, { 0,1,0 });
		gem::vec3 rot = 0;
		gem::vec3 dump = 0;
		gem::decompose(test, dump, rot, dump);
		transform.rotation = rot;
		transform.rotation.y *= -1.0f;
	}
	myMeleeAbility->Update(deltaTime);
	auto t = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/AI/Enemies/melee_enemy/CHR_MeleeEnemy.vtchr");
	auto anim = Volt::AssetManager::Get().GetAsset<Volt::AnimatedCharacter>(t);
	if (myAnimationStarted && myAnimationTime >= anim->GetAnimationDuration(1))
	{
	/*	auto& ac = myEntity.GetComponent<Volt::AnimatedCharacterComponent>();
		ac.currentAnimation = 4;
		ac.isLooping = true;
		myEntity.GetComponent<Volt::AnimatedCharacterComponent>().currentStartTime = Volt::AnimationManager::globalClock;*/
		myAnimationStarted = false;
	}

	// TODO: LOOK AT HOW BRUISER HANDLES ANIM; THIS IS SHIT
	myRangeP = myEntity.GetComponent<Volt::MeleeEnemyComponent>().attackP;
	if (gem::distance(transform.position, target) <= AbilityData::CloseAttackData->size.z * myRangeP)
	{
		if (!myAnimationStarted && myAnimationTime >= anim->GetAnimationDuration(1) && myMeleeAbility->IsCastable())
		{
			auto& ac = myEntity.GetComponent<Volt::AnimatedCharacterComponent>();
			ac.currentAnimation = 1;
			ac.isLooping = false;
			myEntity.GetComponent<Volt::AnimatedCharacterComponent>().currentStartTime = Volt::AnimationManager::globalClock;
			myAnimationTime = 0;
			myAnimationStarted = true;
			myMeleeAbility->CastAfterTime(0.75f);
		}
	}
	else if (!myAnimationStarted)
		SetTransition(eMeleeEnemyState::WALK);

}

void MeleeEnemyAttackState::OnFixedUpdate()
{}

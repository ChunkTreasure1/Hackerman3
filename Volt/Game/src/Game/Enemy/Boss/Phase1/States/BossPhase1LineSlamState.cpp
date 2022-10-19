#include "BossPhase1LineSlamState.h"
#include "Game/Enemy/Boss/BossScript.h"
#include <Volt/Components/Components.h>
#include <Volt/Animation/AnimationManager.h>
#include <Volt/Asset/AssetManager.h>
#include <Volt/Asset/Animation/AnimatedCharacter.h>

BossPhase1LineSlamState::BossPhase1LineSlamState(const Volt::Entity& aEntity) : StateBase(aEntity)
{}

void BossPhase1LineSlamState::OnExit()
{}

void BossPhase1LineSlamState::OnEnter()
{
	gem::vec3 target = { 0.0f, 0.0f, 0.0f };
	bool pFound = false;
	myEntity.GetScene()->GetRegistry().ForEach<Volt::PlayerComponent, Volt::TransformComponent>([&target, &pFound](Wire::EntityId id, const Volt::PlayerComponent& playerComponent, const Volt::TransformComponent& transformComponent)
		{
			target = transformComponent.position;
			pFound = true;
		});
	if (!pFound)
		SetTransition(eBossPhase1State::MAIN);
	auto abilityHandler = myEntity.GetScript<BossScript>("BossScript")->GetAbilityHandler();
	auto abilityEntTransform = abilityHandler->GetEntity();//.GetComponent<Volt::TransformComponent>();
	gem::mat test = gem::lookAt(abilityEntTransform.GetWorldPosition(), target, { 0,1,0 });
	gem::vec3 rot = 0;
	gem::vec3 dump = 0;
	gem::decompose(test, dump, rot, dump);
	abilityEntTransform.GetComponent<Volt::TransformComponent>().rotation = rot;
	abilityEntTransform.GetComponent<Volt::TransformComponent>().rotation.y *= -1.0f;

	myEntity.GetComponent<Volt::AnimatedCharacterComponent>().currentAnimation = 0;
	myEntity.GetComponent<Volt::AnimatedCharacterComponent>().currentStartTime = Volt::AnimationManager::globalClock;
	myAnimationTime = 0;
}

void BossPhase1LineSlamState::OnReset()
{}

void BossPhase1LineSlamState::OnUpdate(const float& deltaTime)
{
	myAnimationTime += deltaTime;
	auto t = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Animations/King/CHR_King.vtchr");
	auto anim = Volt::AssetManager::Get().GetAsset<Volt::AnimatedCharacter>(t);
	auto abilityHandler = myEntity.GetScript<BossScript>("BossScript")->GetAbilityHandler();

	// MAGIC STUFF
	if (myAnimationTime >= 1.5f)
		abilityHandler->Cast(eBossAbility::LINESLAM);

	if (myAnimationTime >= anim->GetAnimationDuration(0))
		SetTransition(eBossPhase1State::MAIN);
}

void BossPhase1LineSlamState::OnFixedUpdate()
{}

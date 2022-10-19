#include "TentacleMainState.h"
#include "Game/Enemy/Boss/Phase2/TentacleScript.h"
#include <Volt/Components/Components.h>
#include "Game/Abilities/AbilityDescriptions.hpp"

TentacleMainState::TentacleMainState(const Volt::Entity& aEntity) : StateBase(aEntity)
{}

void TentacleMainState::OnExit()
{}

void TentacleMainState::OnEnter()
{}

void TentacleMainState::OnReset()
{}

void TentacleMainState::OnUpdate(const float& deltaTime)
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

	gem::mat test = gem::lookAt(myEntity.GetWorldPosition(), target, { 0,1,0 });
	gem::vec3 rot = 0;
	gem::vec3 dump = 0;
	gem::decompose(test, dump, rot, dump);
	myEntity.GetComponent<Volt::TransformComponent>().rotation = rot;
	myEntity.GetComponent<Volt::TransformComponent>().rotation.y *= -1.0f;

	if (myEntity.GetScript<TentacleScript>("TentacleScript")->GetLineSlam()->IsCastable() && gem::distance(target, myEntity.GetWorldPosition()) <= AbilityData::LineSlamData->size.z * 0.7f)
		SetTransition(eTentacleState::ATTACK);
}

void TentacleMainState::OnFixedUpdate()
{}

#include "TentacleAttackState.h"
#include <Volt/Components/Components.h>
#include "Game/Enemy/Boss/Phase2/TentacleScript.h"
#include "Game/Abilities/AbilityDescriptions.hpp"

TentacleAttackState::TentacleAttackState(const Volt::Entity& aEntity) : StateBase(aEntity)
{}

void TentacleAttackState::OnExit()
{}

void TentacleAttackState::OnEnter()
{
	gem::vec3 target = { 0.0f, 0.0f, 0.0f };
	bool pFound = false;
	myEntity.GetScene()->GetRegistry().ForEach<Volt::PlayerComponent, Volt::TransformComponent>([&target, &pFound](Wire::EntityId id, const Volt::PlayerComponent& playerComponent, const Volt::TransformComponent& transformComponent)
		{
			target = transformComponent.position;
			pFound = true;
		});
	if (!pFound)
	{
		SetTransition(eTentacleState::MAIN);
		return;
	}
	gem::mat test = gem::lookAt(myEntity.GetWorldPosition(), target, { 0,1,0 });
	gem::vec3 rot = 0;
	gem::vec3 dump = 0;
	gem::decompose(test, dump, rot, dump);
	myEntity.GetComponent<Volt::TransformComponent>().rotation = rot;
	myEntity.GetComponent<Volt::TransformComponent>().rotation.y *= -1.0f;
	myHasCast = false;
	myTimer = 5;
}

void TentacleAttackState::OnReset()
{}

void TentacleAttackState::OnUpdate(const float& deltaTime)
{
	myTimer -= deltaTime;
	if (myTimer <= 2 && !myHasCast)
	{
		myHasCast = true;
		myEntity.GetScript<TentacleScript>("TentacleScript")->GetLineSlam()->Cast();
	}

	if (myTimer <= 0)
		SetTransition(eTentacleState::MAIN);
}

void TentacleAttackState::OnFixedUpdate()
{}

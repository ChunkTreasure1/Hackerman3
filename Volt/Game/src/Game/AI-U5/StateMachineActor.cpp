#include "StateMachineActor.h"

#include "PollingStationU5.h"
#include "DecisionTreeActor.h"

#include <Volt/Rendering/Renderer.h>
#include <Volt/Physics/Physics.h>
#include <Volt/Physics/PhysicsScene.h>

#include <Volt/Utility/Random.h>

VT_REGISTER_SCRIPT(StateMachineActor);

StateMachineActor::StateMachineActor(Volt::Entity entity)
	: Volt::ScriptBase(entity)
{
	myStateMachine = CreateScope<StateMachine>();
}

void StateMachineActor::OnStart()
{
	// Search state
	{
		auto& state = myStateMachine->AddState();
		state.onUpdate = [&](float aDeltaTime)
		{
			const auto& comp = myEntity.GetComponent<AIU5StateActorComponent>();
			bool hitWall = false;

			// Forward whisker
			{
				Volt::RaycastHit hit;
				if (Volt::Physics::GetScene()->Raycast(myEntity.GetPosition(), gem::normalize(myEntity.GetForward()), comp.maxForwardDistance, &hit, 1))
				{
					hitWall = true;
				}

				const gem::vec3 target = myEntity.GetPosition() + gem::normalize(myEntity.GetForward()) * comp.maxForwardDistance + gem::vec3{ 0.f, 100.f, 0.f };

				Volt::Renderer::SubmitLine(myEntity.GetPosition() + gem::vec3{ 0.f, 100.f, 0.f }, target);
			}

			if (hitWall)
			{
				if (!myIsTurning)
				{
					myTurningDirection = Volt::Random::Float(-1.f, 1.f);
					myIsTurning = true;
				}

				bool leftHit = false;
				bool rightHit = false;

				const float yRot = gem::degrees(myEntity.GetRotation().y) + gem::degrees(90.f);

				// Left whisker
				{
					const gem::vec3 direction = gem::normalize(gem::rotate(gem::quat(gem::vec3{ 0.f, -comp.whiskarAngle, 0.f }), myEntity.GetForward()));

					Volt::RaycastHit hit;
					if (Volt::Physics::GetScene()->Raycast(myEntity.GetPosition(), direction, comp.maxSideDistance, &hit, 1))
					{
						leftHit = true;
					}

					Volt::Renderer::SubmitLine(myEntity.GetPosition() + gem::vec3{ 0.f, 100.f, 0.f }, myEntity.GetPosition() + direction * comp.maxSideDistance + gem::vec3{ 0.f, 100.f, 0.f });
				}

				// Right whisker
				{
					const gem::vec3 direction = gem::normalize(gem::rotate(gem::quat(gem::vec3{ 0.f, comp.whiskarAngle, 0.f }), myEntity.GetForward()));

					Volt::RaycastHit hit;
					if (Volt::Physics::GetScene()->Raycast(myEntity.GetPosition(), direction, comp.maxSideDistance, &hit, 1))
					{
						rightHit = true;
					}

					Volt::Renderer::SubmitLine(myEntity.GetPosition() + gem::vec3{ 0.f, 100.f, 0.f }, myEntity.GetPosition() + direction * comp.maxSideDistance + gem::vec3{ 0.f, 100.f, 0.f });
				}

				// Turn
				{
					if (leftHit && !rightHit)
					{
						myTurningDirection = 1.f;
					}
					else if (rightHit && !leftHit)
					{
						myTurningDirection = -1.f;
					}

					myTurningDirection = myTurningDirection < 0.f ? -1.f : 1.f;
					myEntity.GetPhysicsActor()->SetAngularVelocity({ 0.f, comp.turningSpeed * myTurningDirection, 0.f });
				}

				myEntity.GetPhysicsActor()->SetLinearVelocity(0.f);
			}
			else
			{
				myIsTurning = false;
			}

			if (!hitWall)
			{
				myEntity.GetPhysicsActor()->SetLinearVelocity(gem::normalize(myEntity.GetForward()) * comp.speed);
				myEntity.GetPhysicsActor()->SetAngularVelocity(0.f);
			}
		};

		//state.transitions.emplace_back([&]()
		//	{
		//		const auto& targetPos = PollingStationU5::Get().PollDecisionTargetPosition();
		//		const gem::vec3 direction = gem::normalize(targetPos - myEntity.GetPosition());

		//		Volt::RaycastHit hit;
		//		if (Volt::Physics::GetScene()->Raycast(myEntity.GetPosition(), direction, 10000.f, &hit))
		//		{
		//			Volt::Entity hitEntity = { hit.hitEntity, myEntity.GetScene() };
		//			if (hitEntity.HasComponent<AIU5DecisionActorComponent>())
		//			{
		//				return 1;
		//			}
		//		}

		//		return -1;
		//	});
	}

	myStateMachine->SetStartState(0);
}

void StateMachineActor::OnUpdate(float aDeltaTime)
{
	myStateMachine->Update(aDeltaTime);
}

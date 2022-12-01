#include "StateMachineActor.h"

#include "PollingStationU5.h"
#include "DecisionTreeActor.h"

#include <Volt/Components/Components.h>
#include <Volt/Components//PhysicsComponents.h>
#include <Volt/Rendering/Renderer.h>
#include <Volt/Physics/Physics.h>
#include <Volt/Physics/PhysicsScene.h>

#include <Volt/Utility/Random.h>
#include <Volt/Asset/AssetManager.h>

VT_REGISTER_SCRIPT(StateMachineActor);

StateMachineActor::StateMachineActor(Volt::Entity entity)
	: Volt::ScriptBase(entity)
{
	myStateMachine = CreateScope<StateMachine>();
	myEntity.GetComponent<AIU5HealthComponent>().currentHealth = myEntity.GetComponent<AIU5HealthComponent>().health;
}

void StateMachineActor::OnStart()
{
	// Search state
	{
		auto& state = myStateMachine->AddState();
		state.onEnter = [&]()
		{
			state.blackboard["IsTurning"] = false;
			state.blackboard["TurningDirection"] = 0.f;
		};

		state.onUpdate = [&](float aDeltaTime)
		{
			const auto& comp = myEntity.GetComponent<AIU5StateActorComponent>();
			bool hitWall = false;
			Volt::RaycastHit forwardHit;

			// Forward whisker
			{
				if (Volt::Physics::GetScene()->Raycast(myEntity.GetPosition(), gem::normalize(myEntity.GetForward()), comp.maxForwardDistance, &forwardHit, 1))
				{
					hitWall = true;
				}

				const gem::vec3 target = myEntity.GetPosition() + gem::normalize(myEntity.GetForward()) * comp.maxForwardDistance + gem::vec3{ 0.f, 100.f, 0.f };

				Volt::Renderer::SubmitLine(myEntity.GetPosition() + gem::vec3{ 0.f, 100.f, 0.f }, target);
			}

			if (hitWall)
			{
				if (!std::any_cast<bool>(state.blackboard["IsTurning"]))
				{
					state.blackboard["TurningDirection"] = Volt::Random::Float(-1.f, 1.f);
					state.blackboard["IsTurning"] = true;
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
						state.blackboard["TurningDirection"] = -1.f;
					}
					else if (rightHit && !leftHit)
					{
						state.blackboard["TurningDirection"] = 1.f;
					}

					state.blackboard["TurningDirection"] = std::any_cast<float>(state.blackboard.at("TurningDirection")) < 0.f ? -1.f : 1.f;
					myEntity.GetPhysicsActor()->SetAngularVelocity({ 0.f, comp.turningSpeed * std::any_cast<float>(state.blackboard.at("TurningDirection")), 0.f });
				}

				if (!leftHit && !rightHit && !hitWall)
				{
					state.blackboard["IsTurning"] = false;
				}

				myEntity.GetPhysicsActor()->SetLinearVelocity(0.f);
			}

			if (!hitWall)
			{
				myEntity.GetPhysicsActor()->SetLinearVelocity(gem::normalize(myEntity.GetForward()) * comp.speed);
				myEntity.GetPhysicsActor()->SetAngularVelocity(0.f);
			}
		};

		state.transitions.emplace_back([&]()
			{
				const auto& targetPos = PollingStationU5::Get().PollDecisionTargetPosition();

				const gem::vec3 direction = gem::normalize(targetPos - myEntity.GetPosition());
				const gem::vec3 origin = myEntity.GetPosition() + direction * 300.f;

				Volt::RaycastHit hit;
				if (Volt::Physics::GetScene()->Raycast(origin, direction, 10000.f, &hit))
				{
					Volt::Entity hitEntity = { hit.hitEntity, myEntity.GetScene() };
					if (hitEntity.HasComponent<AIU5DecisionActorComponent>())
					{
						return 1;
					}
				}

				return -1;
			});
	}

	// Attack state
	{
		auto& state = myStateMachine->AddState();
		state.onEnter = [&]()
		{
			state.blackboard.emplace("StartPosition", myEntity.GetPosition());
			state.blackboard.emplace("TimeSinceShot", 10.f);

			const auto& comp = myEntity.GetComponent<AIU5StateActorComponent>();
			myEntity.GetPhysicsActor()->SetLinearVelocity(gem::normalize(myEntity.GetForward()) * comp.speed);
		};

		state.onUpdate = [&](float aDeltaTime)
		{
			// Check walked extra bit
			{
				const gem::vec3 startPosition = std::any_cast<gem::vec3>(state.blackboard.at("StartPosition"));
				if (gem::distance(startPosition, myEntity.GetPosition()) < 1000.f)
				{
					return;
				}
			}

			const auto& comp = myEntity.GetComponent<AIU5StateActorComponent>();

			const auto& targetPos = PollingStationU5::Get().PollDecisionTargetPosition();
			const gem::vec3 direction = gem::normalize(targetPos - myEntity.GetPosition());

			const gem::vec3 rotTo = gem::eulerAngles(gem::fromTo(direction, myEntity.GetForward()));

			if (rotTo.y + myEntity.GetRotation().y < myEntity.GetRotation().y - 0.01)
			{
				myEntity.GetPhysicsActor()->SetAngularVelocity({ 0.f, comp.turningSpeed * -1.f, 0.f });
				myEntity.GetPhysicsActor()->SetLinearVelocity({ 0.f });
			}
			else if (rotTo.y + myEntity.GetRotation().y > myEntity.GetRotation().y + 0.01)
			{
				myEntity.GetPhysicsActor()->SetAngularVelocity({ 0.f, comp.turningSpeed, 0.f });
				myEntity.GetPhysicsActor()->SetLinearVelocity({ 0.f });
			}
			else
			{
				myEntity.GetPhysicsActor()->SetAngularVelocity({ 0.f });

				const float distance = gem::distance(myEntity.GetPosition(), targetPos);
				if (distance > comp.shootDistance)
				{
					myEntity.GetPhysicsActor()->SetLinearVelocity(gem::normalize(direction) * comp.speed);
					return;
				}
				else
				{
					myEntity.GetPhysicsActor()->SetLinearVelocity({ 0.f });
				}

				const float timeSinceShot = std::any_cast<float>(state.blackboard.at("TimeSinceShot"));
				if (timeSinceShot > 1.f / comp.fireRate)
				{
					ShootBullet(direction, 2000.f + comp.speed);
					state.blackboard["TimeSinceShot"] = 0.f;
				}
				else
				{
					state.blackboard["TimeSinceShot"] = timeSinceShot + aDeltaTime;
				}
			}
		};

		state.transitions.emplace_back([&]()
			{
				const auto& targetPos = PollingStationU5::Get().PollDecisionTargetPosition();
				const gem::vec3 direction = gem::normalize(targetPos - myEntity.GetPosition());
				const gem::vec3 origin = myEntity.GetPosition() + direction * 300.f;

				Volt::RaycastHit hit;
				if (Volt::Physics::GetScene()->Raycast(origin, direction, 10000.f, &hit))
				{
					Volt::Entity hitEntity = { hit.hitEntity, myEntity.GetScene() };
					if (!hitEntity.HasComponent<AIU5DecisionActorComponent>())
					{
						return 0;
					}
				}

				return -1;
			});
	}

	myStateMachine->SetStartState(0);
}

void StateMachineActor::OnUpdate(float aDeltaTime)
{
	myStateMachine->Update(aDeltaTime);
}

void StateMachineActor::ShootBullet(const gem::vec3& direction, const float speed)
{
	auto entity = myEntity.GetScene()->CreateEntity();
	entity.SetPosition(myEntity.GetPosition());

	// Mesh
	{
		auto& meshComp = entity.AddComponent<Volt::MeshComponent>();
		meshComp.handle = Volt::AssetManager::GetAsset<Volt::Mesh>("Assets/Meshes/Primitives/Sphere.vtmesh")->handle;
	}

	// Collider
	{
		Volt::RigidbodyComponent ridigComp{};
		ridigComp.disableGravity = true;
		ridigComp.bodyType = Volt::BodyType::Dynamic;

		entity.AddComponent<Volt::RigidbodyComponent>(&ridigComp);
		entity.AddComponent<Volt::SphereColliderComponent>();
	}

	entity.AddScript("BulletScript");
	entity.GetPhysicsActor()->SetLinearVelocity(direction * speed);
}
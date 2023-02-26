#include "StateMachineActor.h"

#include "PollingStationU5.h"
#include "DecisionTreeActor.h"
#include "HealthWellScript.h"

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
	myStartPos = myEntity.GetPosition();
	myStartRot = myEntity.GetRotation();

	PollingStationU5::Get().SetStateRespawn(myStartPos);

	SetVelocity({ Volt::Random::Float(-1.f, 1.f), 0.f, Volt::Random::Float(-1.f, 1.f) });

	// Search state
	{
		auto& state = myStateMachine->AddState();
		state.onEnter = [&]()
		{
			state.blackboard["IsTurning"] = false;
			state.blackboard["TurningDirection"] = 0.f;

			const auto& comp = myEntity.GetComponent<AIU5StateActorComponent>();
			myEntity.GetPhysicsActor()->SetLinearVelocity(myEntity.GetForward() * comp.speed);
			myEntity.GetPhysicsActor()->SetAngularVelocity(0.f);
		};

		state.onUpdate = [&](float aDeltaTime)
		{
			const auto& comp = myEntity.GetComponent<AIU5StateActorComponent>();
			Volt::RaycastHit forwardHit;

			const gem::vec3 directionLeft = gem::normalize(gem::rotate(gem::quat(gem::vec3{ 0.f, -gem::radians(comp.whiskarAngle), 0.f }), myEntity.GetForward()));
			const gem::vec3 directionRight = gem::normalize(gem::rotate(gem::quat(gem::vec3{ 0.f, gem::radians(comp.whiskarAngle), 0.f }), myEntity.GetForward()));

			Volt::RaycastHit leftHit;
			const bool rightWisker = Volt::Physics::GetScene()->Raycast(myEntity.GetPosition(), directionLeft, comp.maxSideDistance, &leftHit, { 0 });

			Volt::RaycastHit rightHit;
			const bool leftWisker = Volt::Physics::GetScene()->Raycast(myEntity.GetPosition(), directionRight, comp.maxSideDistance, &rightHit, { 0 });


			const gem::vec3 currentVelocity = myEntity.GetPhysicsActor()->GetLinearVelocity();
			const gem::vec3 currentDirection = gem::normalize(currentVelocity);
			if (Volt::Physics::GetScene()->Raycast(myEntity.GetPosition(), currentDirection, 10000.f, &forwardHit, { 0 }))
			{
				if (forwardHit.distance < 1000.f)
				{
					const gem::vec3 reflectedDir = gem::reflect(currentDirection, forwardHit.normal);
					SetVelocity(gem::vec3(reflectedDir.x, 0.f, reflectedDir.z));
				}
			}

			if (rightWisker && rightHit.distance < comp.maxSideDistance)
			{
				const gem::vec3 reflectedDir = gem::reflect(directionRight, rightHit.normal);
				SetVelocity(gem::vec3(reflectedDir.x, 0.f, reflectedDir.z));
			}
			if (leftWisker && leftHit.distance < comp.maxSideDistance)
			{
				const gem::vec3 reflectedDir = gem::reflect(directionLeft, leftHit.normal);
				SetVelocity(gem::vec3(reflectedDir.x, 0.f, reflectedDir.z));
			}

			Volt::Renderer::SubmitLine(myEntity.GetPosition(), myEntity.GetPosition() + currentDirection * 1000 + gem::vec3{ 0.f, 100.f, 0.f });
			Volt::Renderer::SubmitLine(myEntity.GetPosition() + gem::vec3{ 0.f, 100.f, 0.f }, myEntity.GetPosition() + directionRight * comp.maxSideDistance + gem::vec3{ 0.f, 100.f, 0.f });
			Volt::Renderer::SubmitLine(myEntity.GetPosition() + gem::vec3{ 0.f, 100.f, 0.f }, myEntity.GetPosition() + directionLeft * comp.maxSideDistance + gem::vec3{ 0.f, 100.f, 0.f });

		};

		state.transitions.emplace_back([&]()
			{
				const auto& targetPos = PollingStationU5::Get().PollDecisionTargetPosition();

				const gem::vec3 direction = gem::normalize(targetPos - myEntity.GetPosition());
				const gem::vec3 origin = myEntity.GetPosition();

				Volt::RaycastHit hit;
				if (Volt::Physics::GetScene()->Raycast(origin, direction, 10000.f, &hit, { 0, 2 }))
				{
					Volt::Entity hitEntity = { hit.hitEntity, myEntity.GetScene() };
					if (hitEntity.HasComponent<AIU5DecisionActorComponent>() && !hitEntity.GetScript<DecisionTreeActor>("DecisionTreeActor")->IsDead())
					{
						return 1;
					}
				}

				return -1;
			});

		state.transitions.emplace_back([&]()
			{
				if (IsHurt())
				{
					return 2;
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
			const gem::vec3 dir = gem::normalize(myEntity.GetForward());
			myEntity.GetPhysicsActor()->SetLinearVelocity(gem::vec3{ dir.x, 0.f, dir.z } *comp.speed);
		};

		state.onUpdate = [&](float aDeltaTime)
		{
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
					const gem::vec3 dir = gem::normalize(direction);
					myEntity.GetPhysicsActor()->SetLinearVelocity(gem::vec3{ dir.x, 0.f, dir.z } *comp.speed);
					return;
				}
				else
				{
					myEntity.GetPhysicsActor()->SetLinearVelocity({ 0.f });
				}

				const float timeSinceShot = std::any_cast<float>(state.blackboard.at("TimeSinceShot"));
				if (timeSinceShot > 1.f / comp.fireRate)
				{
					ShootBullet(direction, 200000.f + comp.speed);
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
				if (Volt::Physics::GetScene()->Raycast(origin, direction, 10000.f, &hit, { 0, 2 }))
				{
					Volt::Entity hitEntity = { hit.hitEntity, myEntity.GetScene() };
					if (!hitEntity.HasComponent<AIU5DecisionActorComponent>() || hitEntity.GetScript<DecisionTreeActor>("DecisionTreeActor")->IsDead())
					{
						return 0;
					}
				}

				return -1;
			});

		state.transitions.emplace_back([&]()
			{
				if (IsHurt())
				{
					return 2;
				}

				return -1;
			});
	}

	// Hurt state
	{
		auto& state = myStateMachine->AddState();

		state.onEnter = [&]()
		{
			state.blackboard["ClosestHealthWell"] = FindClosestHealthWell();

			const auto& comp = myEntity.GetComponent<AIU5StateActorComponent>();
			myEntity.GetPhysicsActor()->SetLinearVelocity(myEntity.GetForward() * comp.speed);
			myEntity.GetPhysicsActor()->SetAngularVelocity(0.f);
		};

		state.onUpdate = [&](float aDeltaTime)
		{
			//myEntity.GetPhysicsActor()->SetAngularVelocity({ 0.f, 0.f , 0.f });

			const gem::vec3 closestHealthWell = FindClosestHealthWell();
			const float distance = gem::distance(myEntity.GetPosition(), closestHealthWell);

			const auto& comp = myEntity.GetComponent<AIU5StateActorComponent>();
			Volt::RaycastHit forwardHit;

			const gem::vec3 currentVelocity = myEntity.GetPhysicsActor()->GetLinearVelocity();
			gem::vec3 currentDirection = gem::normalize(currentVelocity);

			bool clearPathToHealthWell = Volt::Physics::GetScene()->Raycast(myEntity.GetPosition(), gem::normalize(closestHealthWell - myEntity.GetPosition()), gem::distance(closestHealthWell, myEntity.GetPosition()), &forwardHit, { 0 });

			//side wisker
			const float yRot = gem::degrees(myEntity.GetRotation().y) + gem::degrees(90.f);

			bool hitWall = false;

			const gem::vec3 directionLeft = gem::normalize(gem::rotate(gem::quat(gem::vec3{ 0.f, -gem::radians(comp.whiskarAngle), 0.f }), myEntity.GetForward()));
			const gem::vec3 directionRight = gem::normalize(gem::rotate(gem::quat(gem::vec3{ 0.f, gem::radians(comp.whiskarAngle), 0.f }), myEntity.GetForward()));

			Volt::RaycastHit leftHit;
			const bool rightWisker = Volt::Physics::GetScene()->Raycast(myEntity.GetPosition(), directionLeft, comp.maxSideDistance, &leftHit, { 0 });

			Volt::RaycastHit rightHit;
			const bool leftWisker = Volt::Physics::GetScene()->Raycast(myEntity.GetPosition(), directionRight, comp.maxSideDistance, &rightHit, { 0 });


			Volt::Renderer::SubmitLine(myEntity.GetPosition() + gem::vec3{ 0.f, 100.f, 0.f }, myEntity.GetPosition() + directionLeft * comp.maxSideDistance + gem::vec3{ 0.f, 100.f, 0.f });
			Volt::Renderer::SubmitLine(myEntity.GetPosition() + gem::vec3{ 0.f, 100.f, 0.f }, myEntity.GetPosition() + directionRight * comp.maxSideDistance + gem::vec3{ 0.f, 100.f, 0.f });

			if (leftWisker || rightWisker)
			{
				hitWall = true;
			}

			if (Volt::Physics::GetScene()->Raycast(myEntity.GetPosition(), currentDirection, 10000.f, &forwardHit, { 0 }))
			{
				if (forwardHit.distance < 1000.f)
				{
					hitWall = true;
				}

			}

			//
			if (!hitWall)
			{
				currentDirection = gem::normalize(closestHealthWell - myEntity.GetPosition());
				SetVelocity(gem::vec3(currentDirection.x, 0.f, currentDirection.z));
			}

			if (hitWall || myHitWall)
			{
				if (!clearPathToHealthWell)
				{
					if (leftWisker)
					{
						myEntity.GetPhysicsActor()->SetAngularVelocity({ 0.f, comp.turningSpeed * -1.f, 0.f });
					}
					else if (rightWisker)
					{
						myEntity.GetPhysicsActor()->SetAngularVelocity({ 0.f, comp.turningSpeed , 0.f });
					}
				}
				else
				{
					if (leftWisker)
					{

						const gem::vec3 reflectedDir = gem::reflect(directionLeft, leftHit.normal);
						SetVelocity(gem::vec3(reflectedDir.x, 0.f, reflectedDir.z));
					}
					else if (rightWisker)
					{
						const gem::vec3 reflectedDir = gem::reflect(directionRight, rightHit.normal);
						SetVelocity(gem::vec3(reflectedDir.x, 0.f, reflectedDir.z));
					}
					else
					{
						const gem::vec3 reflectedDir = gem::reflect(currentDirection, forwardHit.normal);
						SetVelocity(gem::vec3(reflectedDir.x, 0.f, reflectedDir.z));
					}
				}
			}
		};

		state.transitions.emplace_back([&]()
			{
				const gem::vec3 closestHealthWell = FindClosestHealthWell();

				if (gem::distance(closestHealthWell, myEntity.GetPosition()) < 300.f)
				{
					return 3;
				}

				return -1;
			});
	}

	//Healing State
	{
		auto& state = myStateMachine->AddState();
		state.onEnter = [&]()
		{
			myEntity.GetPhysicsActor()->SetLinearVelocity(0.f);
			myEntity.GetPhysicsActor()->SetAngularVelocity(0.f);
		};

		state.transitions.emplace_back([&]()
			{
				if (!IsHurt())
				{
					return 0;
				}

				return -1;
			});

		state.transitions.emplace_back([&]()
			{
				const auto& targetPos = PollingStationU5::Get().PollDecisionTargetPosition();

				const gem::vec3 direction = gem::normalize(targetPos - myEntity.GetPosition());
				const gem::vec3 origin = myEntity.GetPosition() + direction * 300.f;

				Volt::RaycastHit hit;
				if (Volt::Physics::GetScene()->Raycast(origin, direction, 10000.f, &hit))
				{
					Volt::Entity hitEntity = { hit.hitEntity, myEntity.GetScene() };
					if (hitEntity.HasComponent<AIU5DecisionActorComponent>() && !hitEntity.GetScript<DecisionTreeActor>("DecisionTreeActor")->IsDead())
					{
						return 1;
					}
				}

				return -1;
			});
	}

	// Dodge State
	{
		auto& state = myStateMachine->AddState();
		state.onEnter = [&]()
		{
			myEntity.GetPhysicsActor()->SetLinearVelocity(0.f);
			myEntity.GetPhysicsActor()->SetAngularVelocity(0.f);
		};

		state.onUpdate = [&](float aDeltaTime)
		{


		};

		state.transitions.emplace_back([&]()
			{
				if (!IsHurt())
				{
					return 0;
				}

				return -1;
			});

		state.transitions.emplace_back([&]()
			{

				return -1;
			});

	}


	myStateMachine->SetStartState(0);
}

void StateMachineActor::OnUpdate(float aDeltaTime)
{
	myIsDead = myEntity.GetComponent<AIU5HealthComponent>().currentHealth <= 0;

	if (myIsDead)
	{
		myEntity.GetComponent<Volt::TransformComponent>().visible = false;

		myDeathTimer -= aDeltaTime;
		if (myDeathTimer <= 0.f)
		{
			myEntity.GetComponent<AIU5HealthComponent>().currentHealth = myEntity.GetComponent<AIU5HealthComponent>().health;
			myStateMachine->SetStartState(0);

			myEntity.GetComponent<Volt::TransformComponent>().visible = true;

			myEntity.GetPhysicsActor()->SetAngularVelocity(0.f);
			myEntity.GetPhysicsActor()->SetLinearVelocity(0.f);

			myEntity.SetPosition(PollingStationU5::Get().PollStateRespawnPosition());
			myEntity.SetRotation(myStartRot);
			myDeathTimer = 5.f;
			myIsDead = false;

			const auto& comp = myEntity.GetComponent<AIU5StateActorComponent>();
			myEntity.GetPhysicsActor()->SetLinearVelocity(myEntity.GetForward() * comp.speed);
		}

		return;
	}

	myStateMachine->Update(aDeltaTime);
}

bool StateMachineActor::IsHurt()
{
	const auto& health = myEntity.GetComponent<AIU5HealthComponent>().currentHealth;
	if (!myIsOnHealthWell && health < 40.f)
	{
		return true;
	}

	if (myIsOnHealthWell && health < 80.f)
	{
		return true;
	}

	return false;
}

bool StateMachineActor::IsDead()
{
	return myIsDead;
}

const gem::vec3 StateMachineActor::FindClosestHealthWell()
{
	float closestWellDist = FLT_MAX;
	Volt::Entity closestWell;

	myEntity.GetScene()->GetRegistry().ForEach<HealthWellComponent>([&](Wire::EntityId id, const HealthWellComponent& comp)
		{
			Volt::Entity ent{ id, myEntity.GetScene() };
			const float distance = gem::distance(myEntity.GetPosition(), ent.GetPosition());

			const bool isAvailiable = ent.GetScript<HealthWellScript>("HealthWellScript")->GetPlayer() == Volt::Entity{} || ent.GetScript<HealthWellScript>("HealthWellScript")->GetPlayer() == myEntity;

			if (distance < closestWellDist && isAvailiable)
			{
				closestWell = ent;
				closestWellDist = distance;
			}
		});

	return closestWell.GetPosition();
}

void StateMachineActor::OnTriggerEnter(Volt::Entity entity, bool isTrigger)
{
	if (entity.HasComponent<HealthWellComponent>())
	{
		myIsOnHealthWell = true;
	}
	else if (entity.HasComponent<Volt::RigidbodyComponent>())
	{
		myHitWall = true;
	}
}

void StateMachineActor::OnTriggerExit(Volt::Entity entity, bool isTrigger)
{
	if (entity.HasComponent<HealthWellComponent>())
	{
		myIsOnHealthWell = false;
	}
	else if (entity.HasComponent<Volt::RigidbodyComponent>())
	{
		myHitWall = false;
	}
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
		ridigComp.layerId = 3;
		ridigComp.collisionType = Volt::CollisionDetectionType::Continuous;

		entity.AddComponent<Volt::SphereColliderComponent>();
		entity.AddComponent<Volt::RigidbodyComponent>(&ridigComp);

		entity.SetPosition(myEntity.GetPosition() + direction * 300.f);
	}

	entity.AddScript("BulletScript");
	entity.GetPhysicsActor()->AddForce(direction * speed, Volt::ForceMode::Force);
}

void StateMachineActor::SetVelocity(const gem::vec3& direction)
{
	const auto& comp = myEntity.GetComponent<AIU5StateActorComponent>();
	myEntity.GetPhysicsActor()->SetLinearVelocity(direction * comp.speed);

	const gem::vec3 rot = gem::eulerAngles(gem::quatLookAt(gem::vec3{ direction.x, 0.f, direction.z }, { 0.f, 1.f, 0.f }));
	myEntity.SetRotation(rot);
}
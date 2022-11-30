#include "AIU4Controller.h"

#include "Game/AI-U4/PollingStationU4.h"

#include <Volt/Components/Components.h>
#include <Volt/Utility/Random.h>

#include <numeric>

VT_REGISTER_SCRIPT(AIU4Controller)

AIU4Controller::AIU4Controller(Volt::Entity entity)
	: Volt::ScriptBase(entity)
{}

void AIU4Controller::OnStart()
{
	myCurrentVelocity = { Volt::Random::Float(-500.f, 500.f), 0.f, Volt::Random::Float(-500.f, 500.f) };
}

void AIU4Controller::OnUpdate(float aDeltaTime)
{
	const auto& aiComp = myEntity.GetComponent<AIU4ControllerComponent>();

	auto sepResult = SeparationBehaviour(aDeltaTime);
	auto cohesResult = CohesionBehaviour(aDeltaTime);
	auto velResult = VelocityMatchingResult(aDeltaTime);
	auto wanderResult = WanderBehaviour(aDeltaTime);

	gem::vec3 velocityResult = (sepResult.velocity * aiComp.separationWeight + cohesResult.velocity * aiComp.cohesionWeight + velResult.velocity * aiComp.velocityWeight /*+ wanderResult.velocity * aiComp.wanderWeight*/);
	velocityResult.y = 0.f;

	myCurrentVelocity += velocityResult;

	const float speed = gem::length(myCurrentVelocity);
	if (speed > aiComp.maxSpeed2)
	{
		myCurrentVelocity.x = (myCurrentVelocity.x / speed) * aiComp.maxSpeed2;
		myCurrentVelocity.z = (myCurrentVelocity.z / speed) * aiComp.maxSpeed2;
	}

	myEntity.SetPosition(myEntity.GetPosition() + myCurrentVelocity * aDeltaTime);

	auto currPos = myEntity.GetPosition();
	if (currPos.x > 4000.f)
	{
		myEntity.SetPosition({ -4000.f, currPos.y, currPos.z });
	}
	else if (currPos.x < -4000.f)
	{
		myEntity.SetPosition({ 4000.f, currPos.y, currPos.z });
	}
	else if (currPos.z > 6000.f)
	{
		myEntity.SetPosition({ currPos.x, currPos.y, -6000.f });
	}
	else if (currPos.z < -6000.f)
	{
		myEntity.SetPosition({ currPos.x, currPos.y, 6000.f });
	}

	// lerp i lerp i lerp gameplay stuff
}

const AIU4Controller::BehaviourResult AIU4Controller::SeparationBehaviour(float aDeltaTime)
{
	const auto& aiComp = myEntity.GetComponent<AIU4ControllerComponent>();

	float minDistance = FLT_MAX;
	gem::vec3 minTargetPosition = 0.f;

	const auto& controllerPositions = PollingStationU4::Get().PollControllerPositions();
	for (const auto& ent : controllerPositions)
	{
		const float dist = gem::distance(ent.GetPosition(), myEntity.GetPosition());
		if (dist < aiComp.separationThreshold && dist < minDistance && myEntity != ent)
		{
			minDistance = dist;
			minTargetPosition = ent.GetPosition();
		}
	}

	if (minDistance == FLT_MAX)
	{
		return { 0.f };
	}

	constexpr float speedDecay = 50.f;

	const float strength = gem::min(speedDecay / (minDistance * minDistance), aiComp.maxSpeed);
	const gem::vec3 direction = gem::normalize(myEntity.GetPosition() - minTargetPosition);

	BehaviourResult result{};
	result.velocity = direction * aiComp.maxSpeed;

	return result;
}

const AIU4Controller::BehaviourResult AIU4Controller::CohesionBehaviour(float aDeltaTime)
{
	const auto& aiComp = myEntity.GetComponent<AIU4ControllerComponent>();
	const auto& controllerEntities = PollingStationU4::Get().PollControllerPositions();

	std::vector<Volt::Entity> entitiesInFlock;
	for (const auto& other : controllerEntities)
	{
		const float distance = gem::distance(myEntity.GetPosition(), other.GetPosition());
		if (other != myEntity && distance < aiComp.cohesionThreshold)
		{
			entitiesInFlock.emplace_back(other);
		}
	}

	gem::vec3 midPoint = 0.f;
	for (const auto& ent : entitiesInFlock)
	{
		midPoint += { ent.GetPosition().x, 0.f, ent.GetPosition().z };
	}

	if (entitiesInFlock.empty())
	{
		return {};
	}

	midPoint /= (float)entitiesInFlock.size();

	BehaviourResult result{};
	result.velocity = (midPoint - myEntity.GetPosition());

	return result;
}

const AIU4Controller::BehaviourResult AIU4Controller::VelocityMatchingResult(float aDeltaTime)
{
	auto& controllerEntities = PollingStationU4::Get().PollControllerPositions();
	const auto& aiComp = myEntity.GetComponent<AIU4ControllerComponent>();

	gem::vec3 avgVelocity = 0.f;
	uint32_t count = 0;

	for (auto& ent : controllerEntities)
	{
		const float dist = gem::distance(ent.GetPosition(), myEntity.GetPosition());

		if (dist < aiComp.cohesionThreshold)
		{
			avgVelocity += ent.GetScript<AIU4Controller>("AIU4Controller")->myCurrentVelocity;
			count++;
		}
	}

	if (count > 0)
	{
		avgVelocity /= (float)count;
	}

	BehaviourResult result{};
	result.velocity = (avgVelocity - myCurrentVelocity);

	return result;
}

const AIU4Controller::BehaviourResult AIU4Controller::WanderBehaviour(float aDeltaTime)
{
	const auto& wanderComp = myEntity.GetComponent<AIU4ControllerComponent>();

	if (myTimer <= 0.f)
	{
		myTimer = wanderComp.cooldown;

		myTargetYRotation = myEntity.GetRotation().y + gem::radians(Volt::Random::Float(wanderComp.minRotation, wanderComp.maxRotation));
	}
	else
	{
		myTimer -= aDeltaTime;
	}

	const float rotDir = myEntity.GetRotation().y - myTargetYRotation > 0 ? 1.f : -1.f;
	myEntity.SetRotation({ 0.f, myEntity.GetRotation().y + gem::radians(45.f) * rotDir * aDeltaTime, 0.f });

	BehaviourResult result{};
	result.velocity = myEntity.GetForward() * wanderComp.maxWanderSpeed * aDeltaTime;

	return result;
}

void AIU4Controller::RecursiveCohesionSearch(Volt::Entity entity, std::set<Volt::Entity>& outEntities)
{
	if (outEntities.contains(entity))
	{
		return;
	}

	outEntities.insert(entity);

	const auto& aiComp = entity.GetComponent<AIU4ControllerComponent>();
	const auto& controllerEntities = PollingStationU4::Get().PollControllerPositions();
	for (const auto& otherEnt : controllerEntities)
	{
		const float dist = gem::distance(otherEnt.GetPosition(), entity.GetPosition());

		if (dist < aiComp.cohesionThreshold && otherEnt != entity && !outEntities.contains(otherEnt))
		{
			RecursiveCohesionSearch(otherEnt, outEntities);
		}
	}
}

const AIU4Controller::BehaviourResult AIU4Controller::FindNearestEntity(Volt::Entity entity)
{
	const auto& aiComp = myEntity.GetComponent<AIU4ControllerComponent>();
	const auto& controllerEntities = PollingStationU4::Get().PollControllerPositions();
	
	float minDistance = FLT_MAX;
	gem::vec3 minTargetPosition = 0.f;

	for (const auto& ent : controllerEntities)
	{
		const float dist = gem::distance(ent.GetPosition(), entity.GetPosition());
		if (dist < minDistance && entity != ent)
		{
			minDistance = dist;
			minTargetPosition = ent.GetPosition();
		}
	}

	BehaviourResult result{};
	if (minDistance == FLT_MAX)
	{
		result.velocity = gem::normalize(gem::vec3{ 0.f } - entity.GetPosition()) * aiComp.maxSpeed;
	}
	else
	{
		result.velocity = gem::normalize(minTargetPosition - entity.GetPosition()) * aiComp.maxSpeed;
	}

	return result;
}

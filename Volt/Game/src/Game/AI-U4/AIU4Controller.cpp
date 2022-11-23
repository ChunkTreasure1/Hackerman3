#include "AIU4Controller.h"

#include "Game/AI-U4/PollingStationU4.h"

#include <Volt/Components/Components.h>

#include <numeric>

VT_REGISTER_SCRIPT(AIU4Controller)

AIU4Controller::AIU4Controller(Volt::Entity entity)
	: Volt::ScriptBase(entity)
{}

void AIU4Controller::OnStart()
{
}

void AIU4Controller::OnUpdate(float aDeltaTime)
{
	const auto& aiComp = myEntity.GetComponent<AIU4ControllerComponent>();

	auto sepResult = SeparationBehaviour(aDeltaTime);
	auto cohesResult = CohesionBehaviour(aDeltaTime);
	auto velResult = VelocityMatchingResult(aDeltaTime);

	float weighth1 = (aiComp.separationWeight + aiComp.cohesionWeight) ;
	
	gem::vec3 velocityResult = (sepResult.velocity * aiComp.separationWeight + cohesResult.velocity * aiComp.cohesionWeight + velResult.velocity * aiComp.velocityWeight);
	velocityResult.y = 0.f;

	myEntity.SetPosition(myEntity.GetPosition() + velocityResult * aDeltaTime);

	auto rot = gem::eulerAngles(gem::quatLookAt(gem::normalize(velocityResult), { 0.f, 1.f, 0.f }));
	myEntity.SetRotation({ 0.f, rot.y, 0.f });

	auto currPos = myEntity.GetPosition();
	if (currPos.x > 2500.f)
	{
		myEntity.SetPosition({ -2500.f, currPos.y, currPos.z });
	}
	else if (currPos.x < -2500.f)
	{
		myEntity.SetPosition({ 2500.f, currPos.y, currPos.z });
	}
	else if (currPos.z > 2500.f)
	{
		myEntity.SetPosition({ currPos.x, currPos.y, -2500.f });
	}
	else if (currPos.z < -2500.f)
	{
		myEntity.SetPosition({ currPos.x, currPos.y, 2500.f });
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

	std::set<Volt::Entity> entitiesInFlock;
	RecursiveCohesionSearch(myEntity, entitiesInFlock);

	if (entitiesInFlock.size() == 1)
	{
		return FindNearestEntity(myEntity);
	}

	gem::vec3 midPoint = 0.f;
	for (const auto& ent : entitiesInFlock)
	{
		midPoint += { ent.GetPosition().x, 0.f, ent.GetPosition().z };
	}

	midPoint /= (float)entitiesInFlock.size();

	BehaviourResult result{};
	result.velocity = gem::normalize(midPoint - myEntity.GetPosition()) * aiComp.maxSpeed;

	return result;
}

const AIU4Controller::BehaviourResult AIU4Controller::VelocityMatchingResult(float aDeltaTime)
{


	return BehaviourResult();
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

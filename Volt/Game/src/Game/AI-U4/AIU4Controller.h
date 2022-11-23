#pragma once

#include <Volt/Scripting/ScriptBase.h>
#include <set>

SERIALIZE_COMPONENT((struct AIU4ControllerComponent
{
	PROPERTY(Name = Separation Weight) float separationWeight = 0.33333f;
	PROPERTY(Name = Cohesion Weight) float cohesionWeight = 0.33333f;
	PROPERTY(Name = Velocity Weight) float velocityWeight = 0.33333f;

	PROPERTY(Name = Separation Threshold) float separationThreshold = 500.f;
	PROPERTY(Name = Cohesion Threshold) float cohesionThreshold = 700.f;
	PROPERTY(Name = Max Speed) float maxSpeed = 500.f;

	CREATE_COMPONENT_GUID("{BE1D8091-3500-44C2-8065-A2A569266AE4}"_guid);
}), AIU4ControllerComponent);

class AIU4Controller : public Volt::ScriptBase
{
public:
	AIU4Controller(Volt::Entity entity);

	void OnStart() override;
	void OnUpdate(float aDeltaTime) override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<AIU4Controller>(aEntity); }
	static WireGUID GetStaticGUID() { return "{339FD182-28B0-44C0-8BAC-C28A2F6BDE31}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	struct BehaviourResult
	{
		gem::vec3 velocity;
	};

	const BehaviourResult SeparationBehaviour(float aDeltaTime);
	const BehaviourResult CohesionBehaviour(float aDeltaTime);
	const BehaviourResult VelocityMatchingResult(float aDeltaTime);

	void RecursiveCohesionSearch(Volt::Entity entity, std::set<Volt::Entity>& outEntities);
	const BehaviourResult FindNearestEntity(Volt::Entity entity);
};
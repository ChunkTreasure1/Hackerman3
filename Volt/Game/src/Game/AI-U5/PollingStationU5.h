#pragma once

#include <Volt/Scene/Entity.h>

#include <GEM/gem.h>
#include <vector>

namespace Volt
{
	class Scene;
}

SERIALIZE_COMPONENT((struct AIU5HealthComponent
{
	PROPERTY(Name = Health) float health = 100.f;

	float currentHealth = 0.f;

	CREATE_COMPONENT_GUID("{0607A35B-F3C0-4E50-A8E3-26AAF9D94D46}"_guid);
}), AIU5HealthComponent);

class PollingStationU5
{
public:
	PollingStationU5(Volt::Scene* scene);
	~PollingStationU5();

	void Update(float aDeltaTime);

	const gem::vec3& PollStateTargetPosition() const;
	const gem::vec3& PollDecisionTargetPosition() const;

	inline static PollingStationU5& Get() { return *myInstance; }

private:
	inline static PollingStationU5* myInstance;
	Volt::Scene* myScene;

	gem::vec3 myStateTargetPosition;
	gem::vec3 myDecisionTargetPosition;
};
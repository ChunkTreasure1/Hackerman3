#pragma once

#include <Volt/Scene/Entity.h>

#include <GEM/gem.h>
#include <vector>

namespace Volt
{
	class Scene;
}

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
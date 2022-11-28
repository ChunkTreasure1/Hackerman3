#pragma once

#include <Volt/Scene/Entity.h>

#include <GEM/gem.h>
#include <vector>

namespace Volt
{
	class Scene;
}

class PollingStationU4
{
public:
	PollingStationU4(Volt::Scene* scene);
	~PollingStationU4();

	void Update(float aDeltaTime);

	const gem::vec3& PollTargetPosition() const;
	inline std::vector<Volt::Entity>& PollControllerPositions() { return myControllerPostitions; }

	inline static PollingStationU4& Get() { return *myInstance; }

private:
	inline static PollingStationU4* myInstance;
	Volt::Scene* myScene;

	gem::vec3 myTargetPosition;
	std::vector<Volt::Entity> myControllerPostitions;
};
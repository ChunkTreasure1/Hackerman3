#pragma once

#include <Volt/Scene/Entity.h>

#include <GEM/gem.h>
#include <vector>

namespace Volt
{
	class Scene;
}

class PollingStation
{
public:
	PollingStation(Volt::Scene* scene);
	~PollingStation();

	void Update(float aDeltaTime);

	const gem::vec3& PollTargetPosition() const;
	inline const std::vector<Volt::Entity>& PollControllerPositions() const { return myControllerPostitions; }

	inline static const PollingStation& Get() { return *myInstance; }

private:
	inline static PollingStation* myInstance;
	Volt::Scene* myScene;

	gem::vec3 myTargetPosition;
	std::vector<Volt::Entity> myControllerPostitions;
};
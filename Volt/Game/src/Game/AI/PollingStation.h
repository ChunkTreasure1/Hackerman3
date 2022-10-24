#pragma once

#include <GEM/gem.h>

namespace Volt
{
	class Scene;
}

struct HackPollResult
{
	gem::vec3 target = { 0.f };
	bool isHacked = false;
};

class PollingStation
{
public:
	PollingStation(Volt::Scene* scene);
	~PollingStation();

	void Update(float aDeltaTime);

	const HackPollResult& PollIsBeingHacked() const;
	inline static const PollingStation& Get() { return *myInstance; }

private:
	inline static PollingStation* myInstance;
	Volt::Scene* myScene;

	HackPollResult myCurrentHackResult;
};
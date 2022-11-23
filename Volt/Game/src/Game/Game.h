#pragma once

#include "Core.h"

#include "Game/AI-U4/PollingStationU4.h"

#include <Volt/Core/Base.h>

namespace Volt
{
	class Scene;
}

class AIEventManager;
class PollingStation;

class Game
{
public:
	Game(Volt::Scene* scene);

	void OnStart();
	void OnStop();
	void OnUpdate();

private:
	Ref<AIEventManager> myAIEventManager;
	Ref<PollingStation> myPollingStation;
	Ref<PollingStationU4> myPollingStationU4;

	Volt::Scene* myScene;
};
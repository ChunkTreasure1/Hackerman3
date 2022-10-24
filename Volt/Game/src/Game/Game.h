#pragma once

#include "Core.h"

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

	Volt::Scene* myScene;
};
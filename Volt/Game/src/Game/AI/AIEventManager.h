#pragma once

#include <Volt/Scene/Entity.h>

enum class AIEvent
{
	PanikCompterHak,
	HakStop
};

class AIController;
class AIEventManager
{
public:
	AIEventManager();
	~AIEventManager();

	void Register(AIController* controller);
	void SendEvent(AIEvent event, Volt::Entity entity);

	inline static AIEventManager& Get() { return *myInstance; }

private:
	inline static AIEventManager* myInstance;

	std::vector<AIController*> myRegisteredControllers;
}; 
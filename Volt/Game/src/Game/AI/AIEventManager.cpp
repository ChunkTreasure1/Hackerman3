#include "AIEventManager.h"

#include "AIController.h"

AIEventManager::AIEventManager()
{
	VT_CORE_ASSERT(myInstance == nullptr, "Instance already exists!");
	myInstance = this;
}

AIEventManager::~AIEventManager()
{
	myRegisteredControllers.clear();
	myInstance = nullptr;
}

void AIEventManager::Register(AIController* controller)
{
	myRegisteredControllers.emplace_back(controller);
}

void AIEventManager::SendEvent(AIEvent event, Volt::Entity entity)
{
	for (auto controller : myRegisteredControllers)
	{
		controller->OnAIEvent(event, entity);
	}
}

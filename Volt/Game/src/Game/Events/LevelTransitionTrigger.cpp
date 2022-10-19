#include "LevelTransitionTrigger.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Components/Components.h"

#include "Volt/Core/Application.h"
#include "Volt/Events/ApplicationEvent.h"
#include <Volt/Input/KeyCodes.h>

LevelTransitionTrigger::LevelTransitionTrigger(Volt::Entity entity)
	: ScriptBase(entity)
{

}

void LevelTransitionTrigger::OnEvent(Volt::Event& e)
{
	Volt::EventDispatcher dispatcher(e);

	dispatcher.Dispatch<Volt::KeyPressedEvent>(VT_BIND_EVENT_FN(LevelTransitionTrigger::OnKeyEvent));
}

bool LevelTransitionTrigger::OnKeyEvent(Volt::KeyPressedEvent& e)
{
	if (e.GetKeyCode() == VT_KEY_F9)
	{
		TriggerTransitions();
	}

	return true;
}


void LevelTransitionTrigger::OnTriggerEnter(Volt::Entity entity, bool isTrigger)
{
	//TODO: CHANGE TO PHYSICS LAYER
	if (entity.HasComponent<Volt::PlayerComponent>())
	{
		TriggerTransitions();
	}
}

void LevelTransitionTrigger::TriggerTransitions()
{
	Volt::AssetHandle aHandle = myEntity.GetComponent<Volt::LevelTransitionTriggerComponent>().handle;
	Volt::OnSceneTransitionEvent loadEvent{ aHandle };
	Volt::Application::Get().OnEvent(loadEvent);
}

VT_REGISTER_SCRIPT(LevelTransitionTrigger);


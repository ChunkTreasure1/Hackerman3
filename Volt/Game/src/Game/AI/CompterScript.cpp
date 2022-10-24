#include "CompterScript.h"

#include "AIEventManager.h"

VT_REGISTER_SCRIPT(CompterScript)

CompterScript::CompterScript(Volt::Entity entity)
	: Volt::ScriptBase(entity)
{}

void CompterScript::OnTriggerEnter(Volt::Entity entity, bool isTrigger)
{
	myEntity.GetComponent<CompterComponent>().isPlayerInTrigger = true;
	AIEventManager::Get().SendEvent(AIEvent::PanikCompterHak, myEntity);
}

void CompterScript::OnTriggerExit(Volt::Entity entity, bool isTrigger)
{
	myEntity.GetComponent<CompterComponent>().isPlayerInTrigger = false;
}

#include "DialogueTrigger.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Components/Components.h"
#include "Volt/UI/Layers/DialogueLayer.h"

#include <Volt/Input/KeyCodes.h>

DialogueTrigger::DialogueTrigger(Volt::Entity entity)
	: ScriptBase(entity)
{

}

void DialogueTrigger::TriggerDialogue()
{
	std::string dialoguePath = myEntity.GetComponent<Volt::DialogueTriggerComponent>().DialogueFile;
	dialoguePath = "Assets/UI/Dialogue/" + dialoguePath + ".yaml";
	DialogueLayer::Get().LoadScene(dialoguePath);
}

VT_REGISTER_SCRIPT(DialogueTrigger);
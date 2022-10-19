#include "SetMainMenu.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/UI/UIFunctionRegistry.h"

SetMainMenu::SetMainMenu(Volt::Entity entity)
	: ScriptBase(entity)
{

}

void SetMainMenu::OnStart()
{
	Volt::UIFunctionRegistry::Execute("OpenMainMenu");
}

VT_REGISTER_SCRIPT(SetMainMenu);
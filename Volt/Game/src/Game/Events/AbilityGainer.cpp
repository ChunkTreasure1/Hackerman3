#include "AbilityGainer.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Components/Components.h"

#include "Volt/Core/Application.h"
#include "Volt/Events/ApplicationEvent.h"
#include <Volt/Input/KeyCodes.h>

#include <Volt/Asset/AssetManager.h>
#include "Game/Player/PlayerScript.h"

#include "Volt/UI/Layers/DialogueLayer.h"
#include "Game/Events/DialogueTrigger.h"

#include "Volt/Animation/AnimationManager.h"


AbilityGainer::AbilityGainer(Volt::Entity entity) 
	: ScriptBase(entity)
{

}

void AbilityGainer::OnStart()
{
	myEntity.GetScene()->GetRegistry().ForEach<Volt::PlayerComponent>([&](Wire::EntityId id, const Volt::PlayerComponent& playerComponent)
		{
			myPlayer = Volt::Entity(id, myEntity.GetScene());
		});
}

void AbilityGainer::TriggerGainAbility()
{
	if (myEntity.GetComponent<Volt::AbilityGainerComponent>().Swipe)
	{
		myPlayer.GetScript<PlayerScript>("PlayerScript")->GiveAbility(Swipe);
	}
	if (myEntity.GetComponent<Volt::AbilityGainerComponent>().Dash)
	{
		myPlayer.GetScript<PlayerScript>("PlayerScript")->GiveAbility(Dash);
	}
	if (myEntity.GetComponent<Volt::AbilityGainerComponent>().Fear)
	{
		myPlayer.GetScript<PlayerScript>("PlayerScript")->GiveAbility(Fear);
	}
	if (myEntity.GetComponent<Volt::AbilityGainerComponent>().BloodLust)
	{
		myPlayer.GetScript<PlayerScript>("PlayerScript")->GiveAbility(BloodLust);
	}
	if (myEntity.GetComponent<Volt::AbilityGainerComponent>().BuffedArrow)
	{
		myPlayer.GetScript<PlayerScript>("PlayerScript")->GiveAbility(BuffedArrow);
	}
}

VT_REGISTER_SCRIPT(AbilityGainer);


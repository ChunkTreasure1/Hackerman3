#include "NPC.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Components/Components.h"

#include "Volt/Core/Application.h"
#include "Volt/Events/ApplicationEvent.h"
#include <Volt/Input/KeyCodes.h>

#include <Volt/Asset/AssetManager.h>
#include "Game/Player/PlayerScript.h"

#include "Volt/UI/Layers/DialogueLayer.h"
#include "Game/Events/DialogueTrigger.h"
#include "Game/Events/AbilityGainer.h"

#include "Volt/Animation/AnimationManager.h"

NPC::NPC(Volt::Entity entity)
	: ScriptBase(entity)
{
}

void NPC::OnAwake()
{
	auto& animComp = myEntity.AddComponent<Volt::AnimatedCharacterComponent>();
	animComp.animatedCharacter = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Animations/Duke/CHR_DeepOne.vtchr");

	animComp.currentAnimation = 0;
	animComp.currentStartTime = Volt::AnimationManager::globalClock;

}

void NPC::OnStart()
{
	myEntity.GetScene()->GetRegistry().ForEach<Volt::PlayerComponent>([&](Wire::EntityId id, const Volt::PlayerComponent& playerComponent)
		{
			myPlayer = Volt::Entity(id, myEntity.GetScene());
		});
}

void NPC::OnEvent(Volt::Event& e)
{
	Volt::EventDispatcher dispatcher(e);

	dispatcher.Dispatch<Volt::OnRespawnEvent>(VT_BIND_EVENT_FN(NPC::OnRespawn));
	dispatcher.Dispatch<Volt::KeyPressedEvent>(VT_BIND_EVENT_FN(NPC::OnKeyEvent));
}

bool NPC::OnKeyEvent(Volt::KeyPressedEvent& e)
{
	if (e.GetKeyCode() == VT_KEY_SPACE && myPlayerIsInside == true && !myVisited)
	{
		myEntity.GetScript<AbilityGainer>("AbilityGainer")->TriggerGainAbility();
		myEntity.GetScript<DialogueTrigger>("DialogueTrigger")->TriggerDialogue();

		gem::vec3 myPos = myEntity.GetPosition();
		gem::vec3 aSpawnPoint = myPos;
		aSpawnPoint.x = myEntity.GetForward().x;
		aSpawnPoint.z = myEntity.GetForward().z;
		aSpawnPoint *= -500;
		aSpawnPoint += myPos;


		myPlayer.GetScript<PlayerScript>("PlayerScript")->SetRespawnPosition(aSpawnPoint);
		myVisited = true;
	}

	return false;
}

bool NPC::OnRespawn(Volt::OnRespawnEvent& e)
{
	if (myVisited)
	{
		//PlayRespawnAnimation
		auto& animComp = myEntity.GetComponent<Volt::AnimatedCharacterComponent>();
		animComp.isLooping = false;
		animComp.currentAnimation = 1;
		animComp.currentStartTime = Volt::AnimationManager::globalClock;
	}

	return false;
}

void NPC::OnTriggerEnter(Volt::Entity entity, bool isTrigger)
{
	myPlayerIsInside = true;
}

void NPC::OnTriggerExit(Volt::Entity entity, bool isTrigger)
{
	myPlayerIsInside = false;
}

VT_REGISTER_SCRIPT(NPC);
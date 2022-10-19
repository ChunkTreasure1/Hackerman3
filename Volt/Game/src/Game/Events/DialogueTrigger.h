#pragma once
#include "Volt/Scripting/ScriptBase.h"
#include "Volt/Events/KeyEvent.h"

class DialogueTrigger : public Volt::ScriptBase
{
public:
	DialogueTrigger(Volt::Entity entity);
	~DialogueTrigger() override = default;

	//void OnEvent(Volt::Event& e) override;
	//void OnTriggerEnter(Volt::Entity entity) override;

	void TriggerDialogue();
	//bool OnKeyEvent(Volt::KeyPressedEvent& e);

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<DialogueTrigger>(aEntity); }
	static WireGUID GetStaticGUID() { return "{8CB75D49-36E9-453D-BDC7-7E6FADCA5C0D}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:

};
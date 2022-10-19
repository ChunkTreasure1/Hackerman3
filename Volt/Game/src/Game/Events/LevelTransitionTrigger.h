#pragma once
#include "Volt/Scripting/ScriptBase.h"
#include "Volt/Events/KeyEvent.h"


class LevelTransitionTrigger : public Volt::ScriptBase
{
public:
	LevelTransitionTrigger(Volt::Entity entity);
	~LevelTransitionTrigger() override = default;

	void OnEvent(Volt::Event& e) override;
	void OnTriggerEnter(Volt::Entity entity, bool isTrigger) override;
	//void OnCollisionEnter(Volt::Entity entity) override;

	bool OnKeyEvent(Volt::KeyPressedEvent& e);

	void TriggerTransitions();

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<LevelTransitionTrigger>(aEntity); }
	static WireGUID GetStaticGUID() { return "{E3C5C371-29AB-40E4-9D6F-A3EEBC063BB2}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }


};
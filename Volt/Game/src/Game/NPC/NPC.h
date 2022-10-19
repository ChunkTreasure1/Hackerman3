#pragma once
#include "Volt/Scripting/ScriptBase.h"
#include "Volt/Events/KeyEvent.h"
#include "Volt/Events/GameEvent.h"

class NPC : public Volt::ScriptBase
{
public:
	NPC(Volt::Entity entity);
	~NPC() override = default;

	void OnAwake() override;
	void OnStart() override;

	void OnEvent(Volt::Event& e) override;
	bool OnKeyEvent(Volt::KeyPressedEvent& e);
	bool OnRespawn(Volt::OnRespawnEvent& e);

	void OnTriggerEnter(Volt::Entity entity, bool isTrigger) override;
	void OnTriggerExit(Volt::Entity entity, bool isTrigger) override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<NPC>(aEntity); }
	static WireGUID GetStaticGUID() { return "{251F9014-C7DC-41D9-A964-693E62DCA64A}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	bool myPlayerIsInside = false;
	bool myVisited = false;

	Volt::Entity myPlayer;

};
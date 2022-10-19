#pragma once
#include "Volt/Scripting/ScriptBase.h"
#include "Volt/Events/KeyEvent.h"


class AbilityGainer : public Volt::ScriptBase
{
public:
	AbilityGainer(Volt::Entity entity);
	~AbilityGainer() override = default;

	void OnStart() override;

	void TriggerGainAbility();

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<AbilityGainer>(aEntity); }
	static WireGUID GetStaticGUID() { return "{F0DBBEC7-285E-470A-ACC5-F0A4562EFEBC}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	Volt::Entity myPlayer;
};
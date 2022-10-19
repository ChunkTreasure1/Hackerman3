#pragma once
#include "Volt/Scene/Entity.h"
#include "Volt/Scripting/ScriptBase.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Log/Log.h"

class PlayerPrimaryScript : public Volt::ScriptBase
{
public:
	PlayerPrimaryScript(const Volt::Entity& aEntity);

	void OnTriggerEnter(Volt::Entity entity, bool isTrigger) override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<PlayerPrimaryScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{8E2A297A-E970-444D-BF64-9E828A3647D8}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	void OnAwake() override;
	void OnUpdate(float aDeltaTime) override;
};
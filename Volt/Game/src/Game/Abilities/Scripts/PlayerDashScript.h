#pragma once
#include "Volt/Scene/Entity.h"
#include "Volt/Scripting/ScriptBase.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Log/Log.h"
#include "Volt/Components/Components.h"

class PlayerDashScript : public Volt::ScriptBase
{
public:
	PlayerDashScript(const Volt::Entity& aEntity);

	void OnTriggerEnter(Volt::Entity entity, bool isTrigger) override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<PlayerDashScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{D1F1DAEB-3AE0-4A9F-BB3A-9D9A3CAFF40D}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	void OnAwake() override;
	void OnUpdate(float aDeltaTime) override;
};
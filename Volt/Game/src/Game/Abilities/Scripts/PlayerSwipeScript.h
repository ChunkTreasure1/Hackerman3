#pragma once
#include "Volt/Scene/Entity.h"
#include "Volt/Scripting/ScriptBase.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Log/Log.h"

class PlayerSwipeScript : public Volt::ScriptBase
{
public:
	PlayerSwipeScript(const Volt::Entity& aEntity);

	void OnTriggerEnter(Volt::Entity entity, bool isTrigger) override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<PlayerSwipeScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{A48F9CCA-7CC7-426E-AE86-CF4FE791E8BB}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	void OnAwake() override;
	void OnUpdate(float aDeltaTime) override;
};
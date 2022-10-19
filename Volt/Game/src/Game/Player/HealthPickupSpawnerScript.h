#pragma once
#include "Volt/Scene/Entity.h"
#include "Volt/Scripting/ScriptBase.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Log/Log.h"
#include "Volt/Components/Components.h"

class HealthPickupSpawnerScript : public Volt::ScriptBase
{
public:
	HealthPickupSpawnerScript(const Volt::Entity& aEntity);

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<HealthPickupSpawnerScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{7789F9E1-628E-4C3C-95A1-98C57D04FBAE}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	void OnAwake() override;
	void OnUpdate(float aDeltaTime) override;
};
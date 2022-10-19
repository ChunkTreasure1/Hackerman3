#pragma once
#include "Volt/Scene/Entity.h"
#include "Volt/Scripting/ScriptBase.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Log/Log.h"
#include "Volt/Components/Components.h"

class HealthPickupScript : public Volt::ScriptBase
{
public:
	HealthPickupScript(const Volt::Entity& aEntity);

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<HealthPickupScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{A81B67E0-47E9-4BE0-AD14-273C03AB7FD6}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	Volt::Entity myPlayerTarget;

	float myInitInterpolation = 0.f;
	float myExitInterpolation = 0.f;

	void OnAwake() override;
	void OnUpdate(float aDeltaTime) override;
	void OnFixedUpdate(float aDeltaTime) override;
};
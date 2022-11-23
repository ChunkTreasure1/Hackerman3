#pragma once

#include <Volt/Scripting/ScriptBase.h>

SERIALIZE_COMPONENT((struct WanderControllerComponent
{
	PROPERTY(Name = Speed) float speed = 100.f;
	PROPERTY(Name = Rotation Speed) float rotationSpeed = 5.f;
	PROPERTY(Name = Cooldown) float cooldown = 1.f;
	PROPERTY(Name = Max Rotation) float maxRotation = 10.f;
	PROPERTY(Name = Min Rotation) float minRotation = -10.f;

	CREATE_COMPONENT_GUID("{323EE8EA-5E5F-40FF-A1A2-29D0A1D77C9A}"_guid);
}), WanderControllerComponent);

class WanderController : public Volt::ScriptBase
{
public:
	WanderController(Volt::Entity entity);

	void OnStart() override;
	void OnUpdate(float aDeltaTime) override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<WanderController>(aEntity); }
	static WireGUID GetStaticGUID() { return "{0E3F1726-09EC-40EC-94DC-9241E210D8C2}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	float myTimer = 0.f;
	float myTargetYRotation = 0.f;
};
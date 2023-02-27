#pragma once

#include "StateMachine.h"

#include <Volt/Scripting/ScriptBase.h>

SERIALIZE_COMPONENT((struct AIU5StateActorComponent
{
	PROPERTY(Name = Max Forward Distance) float maxForwardDistance = 300.f;
	PROPERTY(Name = Max Side Distance) float maxSideDistance = 200.f;
	PROPERTY(Name = Speed) float speed = 200.f;
	PROPERTY(Name = Whisker Angle) float whiskarAngle = 60.f;
	PROPERTY(Name = Turning Speed) float turningSpeed = 10.f;

	PROPERTY(Name = Fire Rate) float fireRate = 0.5f;
	PROPERTY(Name = Dodge Cool Down) float coolDown = 5.f;
	PROPERTY(Name = Is Being Attacked) bool Attacked = false;
	PROPERTY(Name = Shoot Distance) float shootDistance = 10000.f;

	CREATE_COMPONENT_GUID("{2E18C468-0ADA-4A88-B0AB-2B93ECCD177D}"_guid);
}), AIU5StateActorComponent);

class StateMachineActor : public Volt::ScriptBase
{
public:
	StateMachineActor(Volt::Entity entity);

	void OnStart() override;
	void OnUpdate(float aDeltaTime) override;

	bool IsHurt();
	bool IsDead();

	const gem::vec3 FindClosestHealthWell();

	void OnTriggerEnter(Volt::Entity entity, bool isTrigger)override ;
	void OnTriggerExit( Volt::Entity entity, bool isTrigger) override;

	void SetIsOnHealthWell(bool avalue) { myIsOnHealthWell = avalue; };

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<StateMachineActor>(aEntity); }
	static WireGUID GetStaticGUID() { return "{5E3DCF3E-690E-4CB7-806B-231BCC1270E0}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	void ShootBullet(const gem::vec3& direction, const float speed);
	void SetVelocity(const gem::vec3& direction);

	bool myIsOnHealthWell = false;
	bool myIsDead = false;
	bool myHitWall = false;
	bool myTurnLeft = false;

	float myAttackedTimer = 0.f;
	float myDeathTimer = 5.f;

	gem::vec3 myStartPos = 0.f;
	gem::vec3 myStartRot = 0.f;
	
	Scope<StateMachine> myStateMachine;
};
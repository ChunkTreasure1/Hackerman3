#pragma once

#include "BrainTree.h"

#include <Volt/Scripting/ScriptBase.h>
#include <any>

SERIALIZE_COMPONENT((struct AIU6BehaviourTreeActorComponent
{
	PROPERTY(Name = Max Forward Distance) float maxForwardDistance = 300.f;
	PROPERTY(Name = Max Side Distance) float maxSideDistance = 200.f;
	PROPERTY(Name = Speed) float speed = 200.f;
	PROPERTY(Name = Whisker Angle) float whiskarAngle = 60.f;
	PROPERTY(Name = Turning Speed) float turningSpeed = 10.f;
	PROPERTY(Name = Fire Rate) float fireRate = 0.5f;
	PROPERTY(Name = Shoot Distance) float shootDistance = 2000.f;
	CREATE_COMPONENT_GUID("{0DF3DB77-90FA-45C4-8CC8-CAA19221F0B4}"_guid);
}), AIU6BehaviourTreeActorComponent);

class BehaviourTreeActor : public Volt::ScriptBase
{
public:
	BehaviourTreeActor(Volt::Entity entity);
	void OnStart() override;
	void OnUpdate(float aDeltaTime) override;
	void OnTriggerEnter(Volt::Entity entity, bool isTrigger)override;
	void OnTriggerExit(Volt::Entity entity, bool isTrigger) override;
	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<BehaviourTreeActor>(aEntity); }
	static WireGUID GetStaticGUID() { return "{2E012D1F-9BA1-44AB-9169-9F86DDDC7442}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }
private:

};


class IsHurt : public BrainTree::Decorator
{
public:
	Status update() override
	{

	}

private:
};
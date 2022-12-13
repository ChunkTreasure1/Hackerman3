#pragma once

#include <Volt/Scripting/ScriptBase.h>
#include <any>

SERIALIZE_COMPONENT((struct AIU5DecisionActorComponent
{
	CREATE_COMPONENT_GUID("{533D850A-DEEB-495B-A10B-E538314412EF}"_guid);
	PROPERTY(Name = Max Forward Distance) float maxForwardDistance = 300.f;
	PROPERTY(Name = Max Side Distance) float maxSideDistance = 200.f;
	PROPERTY(Name = Speed) float speed = 200.f;
	PROPERTY(Name = Whisker Angle) float whiskarAngle = 60.f;
	PROPERTY(Name = Turning Speed) float turningSpeed = 10.f;

	PROPERTY(Name = Fire Rate) float fireRate = 0.5f;
	PROPERTY(Name = Shoot Distance) float shootDistance = 2000.f;
}), AIU5DecisionActorComponent);

class DecisionTreeActor : public Volt::ScriptBase
{
public:
	DecisionTreeActor(Volt::Entity entity);

	void OnStart() override;
	void OnUpdate(float aDeltaTime) override;

	bool IsHealthy();
	bool CanSeeBish();
	void Shoot(float aDeltaTime);
	void Search();

	void GoToHealingPool();
	void KeepHealing();

	void OnTriggerEnter(Volt::Entity entity, bool isTrigger)override;
	void OnTriggerExit(Volt::Entity entity, bool isTrigger) override;

	void ShootBullet(const gem::vec3& direction, const float speed);

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<DecisionTreeActor>(aEntity); }
	static WireGUID GetStaticGUID() { return "{5E3DCF3E-690E-4CB7-806B-231BCC1270E0}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }
	void SetIsOnHealthWell(bool avalue) { myIsOnHealthWell = avalue; };

private:
	bool myIsOnHealthWell = false;
	std::unordered_map<std::string, std::any> blackboard;
};


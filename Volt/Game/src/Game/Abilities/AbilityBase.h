#pragma once
#include <GEM/gem.h>
#include <Volt/Scene/Entity.h>
class AbilityBase
{
public:
	AbilityBase(const Volt::Entity& aEntity) { myEntity = aEntity; }
	void Update(const float& deltaTime);
	bool IsCastable() { return (myCooldownTimer <= 0 && !myWaitingToCast); }
	void SetCooldownTimer(const float& aTime) { myCooldownTimer = aTime; }

	void InitVFXAfterTime(gem::vec3 aPos, gem::vec3 aRot, gem::vec3 aSize, bool aChild, float aInitTime, float aRemoveTime, std::string aPrefabPath);
	Volt::Entity InitVFX(gem::vec3 aPos, gem::vec3 aRot, gem::vec3 aSize, bool aChild,float aRemoveTime, std::string aPrefabPath);

	void CastAfterTime(float aTime);
	virtual void Cast() = 0;
	virtual void CastAt(gem::vec3 aPos) {}
	virtual void Cancel() {};
protected:

	float myCooldownTimer = 0.f;
	float myTimeToVFX = 0.f;
	float myTimeToCast = 0.f;

	gem::vec3 myVFXPos = { 0 };
	gem::vec3 myVFXRot = { 0 };
	gem::vec3 myVFXSize = { 0 };
	bool myVFXIsChild = false;
	float myVFXRemoveTime = 0.f;
	std::string myVFXPath;

	bool myWaitingForVFX = false;
	bool myWaitingToCast = false;
	Volt::Entity myEntity;
};

#include "AbilityBase.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Asset/Prefab.h"
void AbilityBase::Update(const float& deltaTime)
{
	myCooldownTimer -= deltaTime;
	if (myWaitingToCast)
	{
		myTimeToCast -= deltaTime;
	}

	if (myWaitingForVFX)
	{
		myTimeToVFX -= deltaTime;
	}

	if (myTimeToVFX <= 0 && myWaitingForVFX)
	{
		InitVFX(myVFXPos, myVFXRot, myVFXSize, myVFXIsChild, myVFXRemoveTime, myVFXPath);
		myWaitingForVFX = false;
	}

	if (myTimeToCast <= 0 && myWaitingToCast)
	{
		Cast();
		myWaitingToCast = false;
	}
}

void AbilityBase::InitVFXAfterTime(gem::vec3 aPos, gem::vec3 aRot, gem::vec3 aScale, bool aChild, float aInitTime, float aRemoveTime, std::string aPrefabPath)
{
	myVFXPos = aPos;
	myVFXRot = aRot;
	myVFXSize = aScale;
	myVFXRemoveTime = aRemoveTime;
	myVFXPath = aPrefabPath;
	myVFXIsChild = aChild;

	myTimeToVFX = aInitTime;
	myWaitingForVFX = true;
}

Volt::Entity AbilityBase::InitVFX(gem::vec3 aPos, gem::vec3 aRot, gem::vec3 aScale, bool aChild, float aRemoveTime, std::string aPrefabPath)
{
	auto prefab = Volt::AssetManager::GetAsset<Volt::Prefab>(aPrefabPath);

	Wire::EntityId id = prefab->Instantiate(myEntity.GetScene()->GetRegistry());
	Volt::Entity VFX{ id, myEntity.GetScene() };

	if (!VFX.IsNull())
	{
		VFX.SetPosition(aPos);
		VFX.SetRotation(aRot);
		VFX.SetScale(aScale);

		if (aChild) { myEntity.GetScene()->ParentEntity(myEntity, VFX); }

		myEntity.GetScene()->RemoveEntity(VFX, aRemoveTime);

		return VFX;
	}
}

void AbilityBase::CastAfterTime(float aTime)
{
	myTimeToCast = aTime;
	myWaitingToCast = true;
}

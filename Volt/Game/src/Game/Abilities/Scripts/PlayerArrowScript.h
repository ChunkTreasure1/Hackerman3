#pragma once
#include "Volt/Scene/Entity.h"
#include "Volt/Scripting/ScriptBase.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Log/Log.h"

class PlayerArrowScript : public Volt::ScriptBase
{
public:
	PlayerArrowScript(const Volt::Entity& aEntity);

	void OnTriggerEnter(Volt::Entity entity, bool isTrigger) override;

	void IsBuffed(bool aIsBuffed) { myIsBuffed = aIsBuffed; }

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<PlayerArrowScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{20E7314E-14D9-4E3D-98E3-BBA47498B29D}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	bool myIsBuffed = false;

	void SpawnAOE();
	void OnAwake() override;
	void OnUpdate(float aDeltaTime) override;
};
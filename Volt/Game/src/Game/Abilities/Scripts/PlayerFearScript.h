#pragma once
#include "Volt/Scene/Entity.h"
#include "Volt/Scripting/ScriptBase.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Log/Log.h"
#include "Volt/Components/Components.h"

class PlayerFearScript : public Volt::ScriptBase
{
public:
	PlayerFearScript(const Volt::Entity& aEntity);

	void OnTriggerEnter(Volt::Entity entity, bool isTrigger) override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<PlayerFearScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{E0FF2B0D-1C38-4EC1-BA51-B413D4FCC925}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	void OnAwake() override;
	void OnUpdate(float aDeltaTime) override;

	float myInterpolation;
};
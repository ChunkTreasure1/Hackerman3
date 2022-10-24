#pragma once

#include "AIEventManager.h"

#include <Volt/Scripting/ScriptBase.h>

SERIALIZE_COMPONENT((struct AIControllerComponent
{
	PROPERTY(Name = Is Poll Based) bool isPolling = false;

	CREATE_COMPONENT_GUID("{303AE490-9967-4EEF-9A20-CC755E4D209F}"_guid);
}), AIControllerComponent);

class AIController : public Volt::ScriptBase
{
public:
	AIController(Volt::Entity entity);

	void OnStart() override;
	void OnUpdate(float aDeltaTime) override;
	void OnAIEvent(AIEvent event, Volt::Entity entity);

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<AIController>(aEntity); }
	static WireGUID GetStaticGUID() { return "{D3F88DD9-76C8-4CAE-B5A8-A19C55AB76AF}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:

	gem::vec3 myTargetPosition = { 0.f, 0.f, 0.f };
	bool myHasTarget = false;
};
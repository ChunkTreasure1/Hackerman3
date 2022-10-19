#pragma once
#include "Volt/Scripting/ScriptBase.h"

class GameCamera : public Volt::ScriptBase
{
public:
	GameCamera(Volt::Entity entity);
	~GameCamera() override = default;

	void OnAwake() override;
	void OnStart() override;
	void OnEvent(Volt::Event& e) override;
	void OnUpdate(float aDeltaTime) override;
	void OnStop() override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<GameCamera>(aEntity); }
	static WireGUID GetStaticGUID(){ return "{62C8FBEA-89EE-47B1-BD95-DFE2382CDF01}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

	gem::vec3 GetSmoothStepPos(float aDeltaTime);

	Volt::Entity FindPlayerEntity();

private:
	Volt::Entity myTarget;
};


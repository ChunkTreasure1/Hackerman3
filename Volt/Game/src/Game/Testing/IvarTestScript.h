#pragma once

#include <Volt/Scripting/ScriptBase.h>
#include <Volt/Events/KeyEvent.h>

class IvarTestScript : public Volt::ScriptBase
{
public:
	IvarTestScript(Volt::Entity entity);
	~IvarTestScript() override = default;

	void OnAwake() override;
	void OnEvent(Volt::Event& e) override;
	void OnUpdate(float aDeltaTime) override;

	void OnCollisionEnter(Volt::Entity entity) override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<IvarTestScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{75C1EB7F-83D6-4DF4-9238-BACB27BE701E}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	bool myForced = false;
	std::vector<Volt::Entity> myEnities;

	bool OnKeyPressedEvent(Volt::KeyPressedEvent& e);
};
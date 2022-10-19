#pragma once

#include <Volt/Scripting/ScriptBase.h>
#include <Volt/Events/KeyEvent.h>

class NavMeshAgentTest : public Volt::ScriptBase
{
public:
	NavMeshAgentTest(Volt::Entity entity);
	~NavMeshAgentTest() override = default;

	void OnAwake() override;
	void OnEvent(Volt::Event& e) override;
	void OnUpdate(float aDeltaTime) override;

	void OnCollisionEnter(Volt::Entity entity) override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<NavMeshAgentTest>(aEntity); }
	static WireGUID GetStaticGUID() { return "{760B9F1E-89AE-4711-8CFF-299BD35DB64C}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }


private:
	gem::vec3 GetWorldPosFromMouse();
	Volt::Entity GetCameraEntity();

	gem::vec2 myViewportMousePos;
	gem::vec2 myViewportSize;

	bool OnKeyPressedEvent(Volt::KeyPressedEvent& e);
};
#pragma once

#include <Volt/Scripting/ScriptBase.h>
#include <Volt/Events/MouseEvent.h>

class PlayerController : public Volt::ScriptBase
{
public:
	PlayerController(Volt::Entity entity);

	void OnStart() override;
	void OnUpdate(float aDeltaTime) override;
	void OnEvent(Volt::Event& e) override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<PlayerController>(aEntity); }
	static WireGUID GetStaticGUID() { return "{6EF4790E-166C-4DCA-9EFA-B2D2E4BC5C57}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	bool CheckMouseInput(Volt::MouseButtonPressedEvent& e);

	gem::vec3 GetWorldPosFromMouse();

	void UpdateWalkPosition();

	gem::vec3 myTargetPosition = { 0,0,0 };
	gem::vec2 myViewportMousePos;
	gem::vec2 myViewportSize;

	Volt::Entity myCameraEntity;
};
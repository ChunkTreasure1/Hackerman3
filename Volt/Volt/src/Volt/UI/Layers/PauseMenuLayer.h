#pragma once
#include <Volt/Core/Layer/Layer.h>
#include "Volt/UI/Layers/UIBaseLayer.h"

#include <Volt/Events/Event.h>
#include <Volt/Events/ApplicationEvent.h>
#include <Volt/Events/KeyEvent.h>
#include <Volt/Events/MouseEvent.h>

class PauseMenuLayer :public UIBaseLayer
{
public:
	PauseMenuLayer(Ref<Volt::SceneRenderer>& aSceneRenderer);

	bool OnKeyEvent(Volt::KeyPressedEvent& e);
	bool OnRender(Volt::AppRenderEvent& e) override;
	bool OnUpdate(Volt::AppUpdateEvent& e) override;

	void OnResumeGame();

private:

	bool myPauseUIisOpen = false;

};

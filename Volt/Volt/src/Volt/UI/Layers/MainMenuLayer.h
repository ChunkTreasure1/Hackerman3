#pragma once
#include <Volt/Core/Layer/Layer.h>
#include "Volt/UI/Layers/UIBaseLayer.h"

#include <Volt/Events/Event.h>
#include <Volt/Events/ApplicationEvent.h>
#include <Volt/Events/KeyEvent.h>
#include <Volt/Events/MouseEvent.h>

class MainMenuLayer : public UIBaseLayer
{
//FUNCTIONS
public:
	MainMenuLayer(Ref<Volt::SceneRenderer>& aSceneRenderer);

private:
	bool OnKeyEvent(Volt::KeyPressedEvent& e) override;

//VARIABLES
public:

private:

};
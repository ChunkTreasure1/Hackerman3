#pragma once
#include <Volt/Core/Layer/Layer.h>
#include "Volt/UI/Layers/UIBaseLayer.h"

#include <Volt/Events/Event.h>
#include <Volt/Events/ApplicationEvent.h>
#include <Volt/Events/KeyEvent.h>


class HUDLayer : public UIBaseLayer
{
//FUNCTIONS
public:
	HUDLayer(Ref<Volt::SceneRenderer>& aSceneRenderer);
	~HUDLayer() override;

	inline static HUDLayer& Get() { return *myInstance; }

private:
	bool OnKeyEvent(Volt::KeyPressedEvent& e);

//VARIABLES
public:

private:
	inline static HUDLayer* myInstance = nullptr;
};



#pragma once
#include <Volt/Core/Layer/Layer.h>
#include "Volt/UI/Layers/UIBaseLayer.h"

#include <Volt/Events/Event.h>
#include <Volt/Events/ApplicationEvent.h>
#include <Volt/Events/KeyEvent.h>
#include <Volt/Events/MouseEvent.h>


class DebugLayer : public UIBaseLayer
{
	//FUNCTIONS
public:
	DebugLayer(Ref<Volt::SceneRenderer>& aSceneRenderer);
	~DebugLayer() override;

	inline static DebugLayer& Get() { return *myInstance; }

private:
	bool OnKeyEvent(Volt::KeyPressedEvent& e);

	//VARIABLES
public:

private:
	inline static DebugLayer* myInstance = nullptr;
};
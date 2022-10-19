#pragma once
#include <Volt/Core/Layer/Layer.h>
#include "Volt/UI/Layers/UIBaseLayer.h"

#include <Volt/Events/Event.h>
#include <Volt/Events/ApplicationEvent.h>
#include <Volt/Events/KeyEvent.h>
#include <Volt/Events/MouseEvent.h>

class GameOverLayer :public UIBaseLayer
{
public:
	GameOverLayer(Ref<Volt::SceneRenderer>& aSceneRenderer);
private:
};
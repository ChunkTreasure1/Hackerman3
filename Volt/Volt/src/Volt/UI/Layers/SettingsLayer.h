#pragma once
#include <Volt/Core/Layer/Layer.h>
#include "Volt/UI/Layers/UIBaseLayer.h"

#include <Volt/Events/Event.h>
#include <Volt/Events/ApplicationEvent.h>
#include <Volt/Events/KeyEvent.h>

enum ScreenSize
{
	p720,
	p1080,
	COUNT
};

class SettingsLayer : public UIBaseLayer
{
//FUNCTIONS
public:
	SettingsLayer(Ref<Volt::SceneRenderer>& aSceneRenderer);

private:
	void OnChangeResolution(bool isIncrease);
	void OnChangeBorderless();
	void OnChangeFullscreen();

//VARIABLES
public:

private:
	bool isFullScreen = false;
	bool isBorderless = false;

	ScreenSize currentScreenSize = p1080;

};

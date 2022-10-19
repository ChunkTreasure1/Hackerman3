#include "vtpch.h"
#include "HUDLayer.h"

#include <Volt/Core/Base.h>

#include <Volt/Input/KeyCodes.h>
//#include <Volt/Input/MouseButtonCodes.h>
#include <Volt/Input/Input.h>

#include <iostream>

#include "Volt/UI/UILoader.h"

#include "Volt/UI/UIFunctionRegistry.h"
#include "Volt/UI/Layers/DialogueLayer.h"

HUDLayer::HUDLayer(Ref<Volt::SceneRenderer>& aSceneRenderer) : UIBaseLayer(aSceneRenderer)
{
	VT_ASSERT(!myInstance, "HUDLayer already exists");
	myInstance = this;

	renderPass.debugName = "HUD";
	mySettingPath = "Assets/UI/HUD/HUDLayout.yaml";
	LoadUI(mySettingPath.c_str(), myCanvas, mySprites,myButtons,myTexts,myPopups,mySliders);

	std::function<void()> OpenHUD = [this]()
	{
		this->Enable();
	};
	Volt::UIFunctionRegistry::AddFunc("OpenHUD", OpenHUD);

	std::function<void()> CloseHUD = [this]()
	{
		this->Disable();
	};
	Volt::UIFunctionRegistry::AddFunc("CloseHUD", CloseHUD);

	isEnabled = false;
}
	
HUDLayer::~HUDLayer()
{
	myInstance = nullptr;
}

bool HUDLayer::OnKeyEvent(Volt::KeyPressedEvent& e)
{
	if (e.GetKeyCode() == VT_KEY_F8)
	{
		if (isEnabled) { isEnabled = false; }
		else { isEnabled = true; }
	}

	return false;
}

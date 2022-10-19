#include "vtpch.h"
#include "DebugLayer.h"

#include <Volt/Core/Base.h>

#include <Volt/Input/KeyCodes.h>
#include <Volt/Input/MouseButtonCodes.h>
#include <Volt/Input/Input.h>

#include "Volt/UI/UILoader.h"

#include "Volt/UI/UIFunctionRegistry.h"
#include "Volt/UI/Layers/DialogueLayer.h"

DebugLayer::DebugLayer(Ref<Volt::SceneRenderer>& aSceneRenderer) : UIBaseLayer(aSceneRenderer)
{
	VT_ASSERT(!myInstance, "DebugLayer already exists");
	myInstance = this;

	renderPass.debugName = "DebugLayer";
	mySettingPath = "Assets/UI/Debug/DebugLayout.yaml";
	LoadUI(mySettingPath.c_str(), myCanvas, mySprites, myButtons, myTexts, myPopups, mySliders);
	isEnabled = true;
}

DebugLayer::~DebugLayer()
{
	myInstance = nullptr;
}

bool DebugLayer::OnKeyEvent(Volt::KeyPressedEvent& e)
{
	if (e.GetKeyCode() == VT_KEY_F8)
	{
		if (isEnabled) { isEnabled = false; }
		else { isEnabled = true; }
	}

	return false;
}
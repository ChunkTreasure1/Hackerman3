#include "vtpch.h"
#include "LevelSelectLayer.h"

#include <Volt/Core/Base.h>

#include <Volt/Input/KeyCodes.h>
#include <Volt/Input/MouseButtonCodes.h>
#include <Volt/Input/Input.h>

#include "Volt/UI/UILoader.h"
#include "Volt/UI/UIFunctionRegistry.h"

#include <iostream>


LevelSelectLayer::LevelSelectLayer(Ref<Volt::SceneRenderer>& aSceneRenderer) : UIBaseLayer(aSceneRenderer)
{
	mySettingPath = "Assets/UI/MainMenu/LevelSelectLayout.yaml";
	LoadUI(mySettingPath.c_str(), myCanvas, mySprites, myButtons, myTexts, myPopups, mySliders);

	//PlayLevelFunc 
	std::function<void()> openLevelselect = [this]()
	{
		Volt::UIFunctionRegistry::Execute("CloseMainMenu");
		this->Enable();
	};
	Volt::UIFunctionRegistry::AddFunc("OpenLevelSelect", openLevelselect);

	std::function<void()> closeLevelselect = [this]()
	{
		Volt::UIFunctionRegistry::Execute("OpenMainMenu");
		this->Disable();
	};
	Volt::UIFunctionRegistry::AddFunc("CloseLevelSelect", closeLevelselect);

}


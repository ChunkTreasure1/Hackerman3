#include "vtpch.h"
#include "CreditsLayer.h"

#include "Volt/UI/UILoader.h"
#include "Volt/UI/UIFunctionRegistry.h"

CreditsLayer::CreditsLayer(Ref<Volt::SceneRenderer>& aSceneRenderer) : UIBaseLayer(aSceneRenderer)
{
	renderPass.debugName = "CreditsLayer";
	mySettingPath = "Assets/UI/MainMenu/CreditsLayout.yaml";
	LoadUI(mySettingPath.c_str(), myCanvas, mySprites, myButtons, myTexts, myPopups, mySliders);

	std::function<void()> OpenCredits = [this]()
	{
		Volt::UIFunctionRegistry::Execute("CloseMainMenu");
		this->Enable();
	};
	Volt::UIFunctionRegistry::AddFunc("OpenCredits", OpenCredits);

	std::function<void()> CloseCredits = [this]()
	{
		Volt::UIFunctionRegistry::Execute("OpenMainMenu");
		this->Disable();
	};
	Volt::UIFunctionRegistry::AddFunc("CloseCredits", CloseCredits);

}
#include "vtpch.h"
#include "MainMenuLayer.h"

#include <Volt/Core/Base.h>
#include <Volt/Asset/AssetManager.h>

#include <Volt/Input/KeyCodes.h>
#include <Volt/Input/MouseButtonCodes.h>
#include <Volt/Input/Input.h>

#include <iostream>

#include "Volt/UI/UILoader.h"

#include "Volt/UI/UIFunctionRegistry.h"


MainMenuLayer::MainMenuLayer(Ref<Volt::SceneRenderer>& aSceneRenderer) : UIBaseLayer(aSceneRenderer)
{
	renderPass.debugName = "MainMenuLayer";
	mySettingPath = "Assets/UI/MainMenu/MainMenuLayout.yaml";
	LoadUI(mySettingPath.c_str(), myCanvas, mySprites, myButtons, myTexts, myPopups, mySliders);

	Volt::UIFunctionRegistry aFunctionRegistry{};

	//PlayFunc
	std::function<void()> ReturnToMainMenu = [this]()
	{
		Volt::AssetHandle aHandle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Levels/MainMenu/MainMenu.vtscene");
		Volt::OnSceneTransitionEvent loadEvent{ aHandle };
		Volt::Application::Get().OnEvent(loadEvent);
		Volt::UIFunctionRegistry::Execute("SetToMainMenuState");
	};
	Volt::UIFunctionRegistry::AddFunc("ReturnToMainMenu", ReturnToMainMenu);

	std::function<void()> StartLevel1 = [this]()
	{
		Volt::UIFunctionRegistry::Execute("SetToGameState");
		Volt::AssetHandle aHandle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Levels/Level 1 - Intro/Level 1 - Intro.vtscene");
		Volt::OnSceneTransitionEvent loadEvent{ aHandle };
		Volt::Application::Get().OnEvent(loadEvent);
	};
	Volt::UIFunctionRegistry::AddFunc("StartLevel1", StartLevel1);
	
	std::function<void()> StartLevel2 = [this]()
	{
		Volt::UIFunctionRegistry::Execute("SetToGameState");
		Volt::AssetHandle aHandle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Levels/Level 2 - Village/Level 2 - Village.vtscene");
		Volt::OnSceneTransitionEvent loadEvent{ aHandle };
		Volt::Application::Get().OnEvent(loadEvent);
	};
	Volt::UIFunctionRegistry::AddFunc("StartLevel2", StartLevel2);

	std::function<void()> StartLevel3 = [this]()
	{
		Volt::UIFunctionRegistry::Execute("SetToGameState");
		Volt::AssetHandle aHandle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Levels/Level 3 - Swamp/Level 3 - Swamp.vtscene");
		Volt::OnSceneTransitionEvent loadEvent{ aHandle };
		Volt::Application::Get().OnEvent(loadEvent);
	};
	Volt::UIFunctionRegistry::AddFunc("StartLevel3", StartLevel3);

	std::function<void()> StartLevel4 = [this]()
	{
		Volt::UIFunctionRegistry::Execute("SetToGameState");
		Volt::AssetHandle aHandle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Levels/Level 4 - Boss/Level 4 - Boss.vtscene");
		Volt::OnSceneTransitionEvent loadEvent{ aHandle };
		Volt::Application::Get().OnEvent(loadEvent);
	};
	Volt::UIFunctionRegistry::AddFunc("StartLevel4", StartLevel4);
	
	std::function<void()> QuitFunc = [this]()
	{
		Volt::WindowCloseEvent loadEvent{};
		Volt::Application::Get().OnEvent(loadEvent);
		this->Disable();
	};
	Volt::UIFunctionRegistry::AddFunc("Quit", QuitFunc);

	std::function<void()> SetToMainMenuState = [this]()
	{
		Volt::UIFunctionRegistry::Execute("OpenMainMenu");
		Volt::UIFunctionRegistry::Execute("CloseHUD");
		Volt::UIFunctionRegistry::Execute("DisablePause");
		Volt::UIFunctionRegistry::Execute("CloseGameOver");
	};
	Volt::UIFunctionRegistry::AddFunc("SetToMainMenuState", SetToMainMenuState);

	std::function<void()> SetToGameState = [this]()
	{
		Volt::UIFunctionRegistry::Execute("CloseLevelSelect");
		Volt::UIFunctionRegistry::Execute("CloseMainMenu");
		Volt::UIFunctionRegistry::Execute("OpenHUD");
		Volt::UIFunctionRegistry::Execute("EnablePause");
	};
	Volt::UIFunctionRegistry::AddFunc("SetToGameState", SetToGameState);

	std::function<void()> OpenMainMenu = [this]()
	{
		this->Enable();
	};
	Volt::UIFunctionRegistry::AddFunc("OpenMainMenu", OpenMainMenu);

	std::function<void()> CloseMainMenu = [this]()
	{
		this->Disable();
	};
	Volt::UIFunctionRegistry::AddFunc("CloseMainMenu", CloseMainMenu);

}

bool MainMenuLayer::OnKeyEvent(Volt::KeyPressedEvent& e)
{
	if (e.GetKeyCode() == VT_KEY_F9)
	{
		if (isEnabled) { isEnabled = false; }
		else { isEnabled = true; }
	}
	return false;
}





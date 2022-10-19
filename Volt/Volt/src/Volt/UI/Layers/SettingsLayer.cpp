#include "vtpch.h"
#include "SettingsLayer.h"

#include <Volt/Core/Base.h>

#include "Volt/UI/UILoader.h"
#include "Volt/UI/UIFunctionRegistry.h"
#include "Volt/Events/ApplicationEvent.h"

#include <Volt/Core/Application.h>


SettingsLayer::SettingsLayer(Ref<Volt::SceneRenderer>& aSceneRenderer) : UIBaseLayer(aSceneRenderer)
{
	renderPass.debugName = "SettingsLayer";
	mySettingPath = "Assets/UI/MainMenu/SettingsLayout.yaml";
	LoadUI(mySettingPath.c_str(), myCanvas, mySprites, myButtons, myTexts, myPopups, mySliders);

	std::function<void()> IncreaseResolution = [this]()
	{
		OnChangeResolution(true);
	};
	Volt::UIFunctionRegistry::AddFunc("IncreaseResolution", IncreaseResolution);


	std::function<void()> DecreaseResolution = [this]()
	{
		OnChangeResolution(false);
	};
	Volt::UIFunctionRegistry::AddFunc("DecreaseResolution", DecreaseResolution);


	std::function<void()> ChangeBorderless = [this]()
	{
		OnChangeBorderless();
	};
	Volt::UIFunctionRegistry::AddFunc("ChangeBorderless", ChangeBorderless);

	std::function<void()> ChangeFullsceen = [this]()
	{
		OnChangeFullscreen();
	};
	Volt::UIFunctionRegistry::AddFunc("ChangeFullscreen", ChangeFullsceen);

	std::function<void()> OpenSettings = [this]()
	{
		Volt::UIFunctionRegistry::Execute("CloseMainMenu");
		this->Enable();
	};
	Volt::UIFunctionRegistry::AddFunc("OpenSettings", OpenSettings);

	std::function<void()> CloseSettings = [this]()
	{
		Volt::UIFunctionRegistry::Execute("SetToMainMenuState");
		this->Disable();
	};
	Volt::UIFunctionRegistry::AddFunc("CloseSettings", CloseSettings);

}

void SettingsLayer::OnChangeResolution(bool isIncrease)
{
	bool FAILED = true;
	int currentScreenNR = (int)currentScreenSize;
	if (isIncrease)
	{
		currentScreenNR++;
		if (currentScreenNR < COUNT)
		{
			currentScreenSize = (ScreenSize)currentScreenNR;
			FAILED = false;
		}
	}
	else
	{
		currentScreenNR--;
		if (currentScreenNR >= 0)
		{
			currentScreenSize = (ScreenSize)currentScreenNR;
			FAILED = false;
		}
	}

	if (FAILED) { return; }
	if (currentScreenSize == p720)
	{
		Volt::WindowResizeEvent loadEvent{ 1280, 720 };
		Volt::Application::Get().OnEvent(loadEvent);
	}
	else if (currentScreenSize == p1080)
	{
		Volt::WindowResizeEvent loadEvent{ 1920, 1080};
		Volt::Application::Get().OnEvent(loadEvent);
	}
}

void SettingsLayer::OnChangeBorderless()
{
	if (isBorderless)
	{
		Volt::Application::Get().GetWindow().SetWindowMode(Volt::WindowMode::Borderless);
		isBorderless = true;
	}
	else
	{
		Volt::Application::Get().GetWindow().SetWindowMode(Volt::WindowMode::Windowed);
		isBorderless = false;
	}
	
}

void SettingsLayer::OnChangeFullscreen()
{
	if (isFullScreen)
	{
		Volt::Application::Get().GetWindow().SetWindowMode(Volt::WindowMode::Windowed);
		isFullScreen = true;
	}
	else
	{
		Volt::Application::Get().GetWindow().SetWindowMode(Volt::WindowMode::Fullscreen);
		isFullScreen = false;
	}
}

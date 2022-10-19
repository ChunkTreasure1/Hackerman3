#include "vtpch.h"
#include "GameOverLayer.h"

#include "Volt/Core/Application.h"
#include "Volt/Events/GameEvent.h"

#include "Volt/UI/UILoader.h"

GameOverLayer::GameOverLayer(Ref<Volt::SceneRenderer>& aSceneRenderer) : UIBaseLayer(aSceneRenderer)
{
	mySettingPath = "Assets/UI/GameOver/GameOverLayout.yaml";
	LoadUI(mySettingPath.c_str(), myCanvas, mySprites, myButtons, myTexts, myPopups, mySliders);

	std::function<void()> RespawnFunc = [this]() { 
		Volt::OnRespawnEvent loadEvent{};
		Volt::Application::Get().OnEvent(loadEvent);
		this->Disable(); 
	};
	Volt::UIFunctionRegistry::AddFunc("Respawn", RespawnFunc);

	std::function<void()> OpenGameOver = [this]()
	{
		this->Enable();
		Volt::UIFunctionRegistry::Execute("DisablePause");
	};
	Volt::UIFunctionRegistry::AddFunc("OnGameOver", OpenGameOver);

	std::function<void()> CloseGameOver = [this]()
	{
		this->Disable();
	};
	Volt::UIFunctionRegistry::AddFunc("CloseGameOver", CloseGameOver);

}


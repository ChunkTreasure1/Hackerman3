#include "Launcher/GameLayer.h"
#include "Launcher/FinalLayer.h"

#include "Volt/UI/Layers/HUDLayer.h"
#include "Volt/UI/Layers/PauseMenuLayer.h"
#include "Volt/UI/Layers/DialogueLayer.h"
#include "Volt/UI/Layers/GameOverLayer.h"
#include "Volt/UI/Layers/MainMenuLayer.h"
#include "Volt/UI/Layers/LevelSelectLayer.h"
#include "Volt/UI/Layers/SettingsLayer.h"

#include <Volt/UI/Layers/HUDLayer.h>
#include <Volt/UI/Layers/PauseMenuLayer.h>

#include <Volt/EntryPoint.h>
#include <Volt/Core/Application.h>

class LauncherApp : public Volt::Application
{
public:
	LauncherApp(const Volt::ApplicationInfo& appInfo)
		: Volt::Application(appInfo)
	{
		GameLayer* game = new GameLayer();
		PushLayer(game);
		PushLayer(new FinalLayer(game->GetSceneRenderer()));
		game->LoadStartScene();
	}

private:
};

Volt::Application* Volt::CreateApplication()
{
	Volt::ApplicationInfo info{};
	info.enableImGui = false;
	info.width = 1920;
	info.height = 1080;
	info.title = "Spite";
	info.useVSync = true;
	info.windowMode = WindowMode::Windowed;
	info.isRuntime = true;
	
	return new LauncherApp(info);
}
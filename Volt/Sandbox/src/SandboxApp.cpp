#include "sbpch.h"

#include "Sandbox/Sandbox.h"
#include "Volt/UI/Layers/DebugLayer.h"
#include "Volt/UI/Layers/MainMenuLayer.h"
#include "Volt/UI/Layers/LevelSelectLayer.h"
#include "Volt/UI/Layers/SettingsLayer.h"
#include "Volt/UI/Layers/CreditsLayer.h"
#include "Volt/UI/Layers/HUDLayer.h"
#include "Volt/UI/Layers/DialogueLayer.h"
#include "Volt/UI/Layers/PauseMenuLayer.h"
#include "Volt/UI/Layers/GameOverLayer.h"

#include <Volt/Core/Application.h>
#include <Volt/EntryPoint.h>

class SandboxApp : public Volt::Application 
{
public:
	SandboxApp(const Volt::ApplicationInfo& appInfo)
		: Volt::Application(appInfo)
	{
		Sandbox* sandbox = new Sandbox();

		PushLayer(sandbox);
	}
};

Volt::Application* Volt::CreateApplication()
{
	Volt::ApplicationInfo info{};
	return new SandboxApp(info);
}
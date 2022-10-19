#pragma once

#include "Volt/Core/Window.h"
#include "Volt/Core/Base.h"
#include "Volt/Core/Layer/LayerStack.h"

#include "Volt/Core/Layer/LayerStack.h"
#include "Volt/Events/ApplicationEvent.h"

#include <string>

#include "../../../Game/src/Game/Abilities/AbilityDescriptions.hpp" // this is very funky :)

class AudioEngine;

namespace Volt
{
	struct ApplicationInfo
	{
		ApplicationInfo(const std::string& aTitle = "Volt", WindowMode aWindowMode = WindowMode::Windowed, uint32_t aWidth = 1280, uint32_t aHeight = 720, bool aUseVSync = true, bool aEnableImGui = true)
			: title(aTitle), width(aWidth), height(aHeight), useVSync(aUseVSync), enableImGui(aEnableImGui), windowMode(aWindowMode)
		{}

		std::string title;
		WindowMode windowMode;
		uint32_t width;
		uint32_t height;
		bool useVSync;
		bool enableImGui;
		bool isRuntime = false;
	};

	class ImGuiImplementation;
	class AssetManager;
	class Application
	{
	public:
		Application(const ApplicationInfo& info = ApplicationInfo());
		virtual ~Application();

		void Run();
		void OnEvent(Event& event);

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		inline Window& GetWindow() const { return *myWindow; }
		inline static Application& Get() { return *myInstance; }

		inline const bool IsRuntime() const { return myInfo.isRuntime; }

	private:
		bool OnWindowCloseEvent(WindowCloseEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);

		bool myIsRunning = false;
		bool myIsMinimized = false;

		bool myShouldFancyOpen = false;
		bool myShouldFancyClose = false;
		
		float myFancyCloseTimer = 0.5f;
		const float myFancyCloseTime = 0.5f;

		float myFancyOpenTimer = 0.5f;
		const float myFancyOpenTime = 0.5f;

		float myCurrentFrameTime = 0.f;
		float myLastFrameTime = 0.f;

		ApplicationInfo myInfo;
		inline static Application* myInstance;

		LayerStack myLayerStack;

		Scope<AssetManager> myAssetManager;
		Scope<Window> myWindow;
		Scope<ImGuiImplementation> myImGuiImplementation;

		Ref<AbilityData> myAbilityData;
	};

	static Application* CreateApplication();
}
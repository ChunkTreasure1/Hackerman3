#include "GameLayer.h"

#include <Volt/Scene/Scene.h>
#include <Volt/Asset/AssetManager.h>

#include <Volt/Rendering/SceneRenderer.h>
#include <Volt/Rendering/Framebuffer.h>

#include <Volt/Core/Application.h>

void GameLayer::OnAttach()
{
	myScene = Volt::AssetManager::GetAsset<Volt::Scene>("Assets/Levels/AIU4Level/AIU4Level.vtscene");
	mySceneRenderer = CreateRef<Volt::SceneRenderer>(myScene);
}

void GameLayer::OnDetach()
{
	myScene->OnRuntimeEnd();
	myGame->OnStop();

	mySceneRenderer = nullptr;
	myScene = nullptr;
}

void GameLayer::OnEvent(Volt::Event & e)
{
	Volt::EventDispatcher dispatcher{ e };
	dispatcher.Dispatch<Volt::AppUpdateEvent>(VT_BIND_EVENT_FN(GameLayer::OnUpdateEvent));
	dispatcher.Dispatch<Volt::AppRenderEvent>(VT_BIND_EVENT_FN(GameLayer::OnRenderEvent));
	dispatcher.Dispatch<Volt::WindowResizeEvent>(VT_BIND_EVENT_FN(GameLayer::OnWindowResizeEvent));
	dispatcher.Dispatch<Volt::OnSceneTransitionEvent>(VT_BIND_EVENT_FN(GameLayer::OnSceneTransition));

	myScene->OnEvent(e);
}

void GameLayer::LoadStartScene()
{
	myGame = CreateRef<Game>(myScene.get());
	myGame->OnStart();
	myScene->OnRuntimeStart();
	Volt::OnScenePlayEvent playEvent{};
	Volt::Application::Get().OnEvent(playEvent);

	Volt::Application::Get().GetWindow().Maximize();
}

bool GameLayer::OnUpdateEvent(Volt::AppUpdateEvent & e)
{
	myGame->OnUpdate();
	myScene->Update(e.GetTimestep());

	if (myShouldLoadNewScene)
	{
		TransitionToNewScene();
	}

	return false;
}

bool GameLayer::OnRenderEvent(Volt::AppRenderEvent& e)
{
	mySceneRenderer->OnRenderRuntime();
	return false;
}

bool GameLayer::OnWindowResizeEvent(Volt::WindowResizeEvent& e)
{
	mySceneRenderer->Resize(e.GetWidth(), e.GetHeight());
	myScene->SetRenderSize(e.GetWidth(), e.GetHeight());
	return false;
}

bool GameLayer::OnSceneTransition(Volt::OnSceneTransitionEvent& e)
{
	myStoredScene = Volt::AssetManager::GetAsset<Volt::Scene>(e.GetHandle());
	myShouldLoadNewScene = true;

	return true;
}

void GameLayer::TransitionToNewScene()
{
	myScene->OnRuntimeEnd();

	Volt::AssetManager::Get().Unload(myScene->handle);

	myScene = myStoredScene;
	mySceneRenderer = CreateRef<Volt::SceneRenderer>(myScene);

	Volt::OnSceneLoadedEvent loadEvent{ myScene };
	Volt::Application::Get().OnEvent(loadEvent);

	myScene->OnRuntimeStart();

	Volt::WindowResizeEvent windowResizeEvent{ mySceneRenderer->GetFinalFramebuffer()->GetWidth(), mySceneRenderer->GetFinalFramebuffer()->GetHeight() };
	Volt::Application::Get().OnEvent(windowResizeEvent);

	Volt::OnScenePlayEvent playEvent{};
	Volt::Application::Get().OnEvent(playEvent);

	myShouldLoadNewScene = false;
}

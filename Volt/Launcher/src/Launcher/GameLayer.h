#pragma once

#include <Volt/Core/Layer/Layer.h>
#include <Volt/Events/ApplicationEvent.h>

#include <Game/Game.h>

namespace Volt
{
	class SceneRenderer;
	class Scene;
}

class GameLayer : public Volt::Layer
{
public:
	GameLayer() = default;
	~GameLayer() override = default;

	void OnAttach() override;
	void OnDetach() override;

	void OnEvent(Volt::Event& e) override;

	void LoadStartScene(); // remove

	Ref<Volt::SceneRenderer>& GetSceneRenderer() { return mySceneRenderer; }

private:
	bool OnUpdateEvent(Volt::AppUpdateEvent& e);
	bool OnRenderEvent(Volt::AppRenderEvent& e);
	bool OnWindowResizeEvent(Volt::WindowResizeEvent& e);
	bool OnSceneTransition(Volt::OnSceneTransitionEvent& e);

	void TransitionToNewScene();

	Ref<Volt::SceneRenderer> mySceneRenderer;
	Ref<Volt::Scene> myScene;
	Ref<Game> myGame;

	Ref<Volt::Scene> myStoredScene;
	bool myShouldLoadNewScene = false;
};
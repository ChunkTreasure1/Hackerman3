#pragma once
#include <Volt/Core/Layer/Layer.h>
#include "Volt/UI/Layers/UIBaseLayer.h"

#include <Volt/Events/Event.h>
#include <Volt/Events/ApplicationEvent.h>
#include <Volt/Events/KeyEvent.h>
#include <Volt/Events/MouseEvent.h>

#include <queue>

class DialogueLayer :public UIBaseLayer
{
public:
	DialogueLayer(Ref<Volt::SceneRenderer>& aSceneRenderer);
	~DialogueLayer() override;

	inline static DialogueLayer& Get() { return *myInstance; }

	bool OnKeyEvent(Volt::KeyPressedEvent& e) override;
	bool OnRender(Volt::AppRenderEvent& e) override;
	bool OnMouseEvent(Volt::MouseButtonPressedEvent& e) override;

	void LoadScene(std::string aPath);

private:
	void NextDialogue();

private:
	inline static DialogueLayer* myInstance = nullptr;

	std::queue<std::string> myLoadedDialogue;

};
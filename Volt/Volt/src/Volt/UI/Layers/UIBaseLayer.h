#pragma once
#include <Volt/Core/Layer/Layer.h>
#include <Volt/Events/Event.h>
#include <Volt/Rendering/SceneRenderer.h>

#include <Volt/Asset/Video/Video.h>

#include <Volt/Events/ApplicationEvent.h>

#include <Volt/Events/MouseEvent.h>
#include <Volt/Events/KeyEvent.h>

#include "Volt/UI/UISprite.h"
#include "Volt/UI/UIButton.hpp"
#include "Volt/UI/UIText.h"
#include "Volt/UI/UIPopUp.h"
#include "Volt/UI/UISlider.h"

#include "Volt/UI/UICanvas.h"


class UIBaseLayer : public Volt::Layer
{
	//FUNCTIONS
public:
	UIBaseLayer(Ref<Volt::SceneRenderer>& aSceneRenderer);
	virtual ~UIBaseLayer() = default;

	virtual void Enable();
	virtual void Disable();

private:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(Volt::Event& e) override;

	virtual bool OnRender(Volt::AppRenderEvent& e);
	virtual bool OnUpdate(Volt::AppUpdateEvent& e);

	virtual bool OnPlay(Volt::OnScenePlayEvent& e);
	virtual bool OnStop(Volt::OnSceneStopEvent& e);
	virtual bool OnKeyEvent(Volt::KeyPressedEvent& e);
	virtual bool OnMouseEvent(Volt::MouseButtonPressedEvent& e);
	bool OnViewportResize(Volt::ViewportResizeEvent& e);
	bool OnWindowResize(Volt::WindowResizeEvent& e);

	//VARIABLES
protected:
	bool isEnabled = false;
	Ref<UICanvas> myCanvas;

	Ref<Volt::Camera> camera;
	Ref<Volt::Shader> screenspaceShader;
	Volt::RenderPass renderPass;
	Ref<Volt::SceneRenderer>& myRenderpassRef;

	Ref<Volt::Video> myTestVideo;

	std::vector<UISprite> mySprites;
	std::vector<UIButton> myButtons;
	std::vector<UIText> myTexts;
	std::vector<UIPopUp> myPopups;
	std::vector<UISlider> mySliders;

	std::string mySettingPath;
	bool isMousePressed = false;
	bool PopFromLayer = false;

private:
};
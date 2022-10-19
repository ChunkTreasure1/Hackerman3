#pragma once

#include "Sandbox/Window/EditorWindow.h"
#include "Sandbox/Utility/EditorUtilities.h"

#include <Volt/Rendering/RenderPass.h>
#include <Volt/Events/ApplicationEvent.h>
#include <Volt/Asset/Asset.h>

#include <GEM/gem.h>

namespace Volt
{
	class Framebuffer;
	class Shader;
	class AnimatedCharacter;
	class ComputePipeline;
}

class EditorCameraController;
class CharacterEditorPanel : public EditorWindow
{
public:
	CharacterEditorPanel();
	~CharacterEditorPanel() override = default;

	void UpdateMainContent() override;
	void UpdateContent() override;
	void OnEvent(Volt::Event& e) override;
	void OpenAsset(Ref<Volt::Asset> asset) override;

private:
	bool OnRenderEvent(Volt::AppRenderEvent& e);
	bool OnUpdateEvent(Volt::AppUpdateEvent& e);

	void UpdateToolbar();
	void UpdateViewport();
	void UpdateProperties();
	void UpdateAnimations();

	Volt::RenderPass myForwardPass;

	Ref<Volt::Shader> myGridShader;
	Ref<Volt::AnimatedCharacter> myCurrentCharacter;
	Ref<EditorCameraController> myCameraController;

	gem::vec2 myPerspectiveBounds[2] = { { 0.f, 0.f }, { 0.f, 0.f } };
	gem::vec2 myViewportSize = { 1280.f, 720.f };

	const float myButtonSize = 22.f;
	float myCurrentAnimStartTime = 0.f;

	bool myIsPlayingAnim = false;
	bool myIsLooping = true;
	uint32_t myCurrentAnimPlaying = 0;

	NewCharacterData myNewCharacterData{};
};
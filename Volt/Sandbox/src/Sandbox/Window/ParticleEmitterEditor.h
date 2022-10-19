#pragma once
#include "Sandbox/Window/EditorWindow.h"

#include <Volt/Core/Base.h>
#include <Volt/Scene/Scene.h>
#include <Volt/Rendering/RenderPass.h>
#include <Volt/Events/ApplicationEvent.h>
#include <Volt/Particles/Particle.h>

#include <vector>

namespace Volt
{
	class Shader;
	class ParticlePreset;
}

class EditorCameraController;
class ParticleEmitterEditor : public EditorWindow
{
public:
	ParticleEmitterEditor();
	void UpdateMainContent() override;
	void OpenAsset(Ref<Volt::Asset> asset) override;
	bool SavePreset(const std::filesystem::path& indata);

	void OnEvent(Volt::Event& e) override;

private:
	struct ParticleSystemData
	{
		float emittionTimer = 0;
		int numberOfAliveParticles = 0;
		std::vector<Volt::Particle> particles;
	};

	bool OnRenderEvent(Volt::AppRenderEvent& e);
	bool OnUpdateEvent(Volt::AppUpdateEvent& e);

	void UpdateViewport();
	void UpdateProperties();

	void SendParticles(float aDeltaTime);
	void ParticlePositionUpdate(Volt::Particle& aParticle, float aDeltaTime);
	bool ParticleKillCheck(Volt::Particle& aParticle);

	void OpenParticleSystem(const std::filesystem::path& aPath);

	ParticleSystemData myParticleSystemData;

	std::vector<std::string> myPresets;

	gem::vec2 myPerspectiveBounds[2] = { { 0.f, 0.f }, { 0.f, 0.f } };
	gem::vec2 myViewportSize = { 1280.f, 720.f };
	
	Volt::RenderPass myParticlePass;
	Ref<Volt::Shader> myGridShader;
	Ref<Volt::ParticlePreset> myCurrentPreset;

	Ref<EditorCameraController> myCameraController;
};
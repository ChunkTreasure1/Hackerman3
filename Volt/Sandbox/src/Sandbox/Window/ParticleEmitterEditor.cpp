#include "sbpch.h"
#include "ParticleEmitterEditor.h"

#include "Sandbox/Camera/EditorCameraController.h"

#include <Volt/Asset/ParticlePreset.h>
#include <Volt/Asset/AssetManager.h>

#include <Volt/Rendering/Renderer.h>
#include <Volt/Rendering/Framebuffer.h>
#include <Volt/Rendering/Shader/ShaderRegistry.h>
#include <Volt/Rendering/Shader/Shader.h>

#include <Volt/Utility/UIUtility.h>

#include <random>


ParticleEmitterEditor::ParticleEmitterEditor()
	: EditorWindow("ParticleSystem Preset Editor")
{
	myCameraController = CreateRef<EditorCameraController>(60.f, 1.f, 100000.f);
	myGridShader = Volt::ShaderRegistry::Get("Grid");

	// Forward
	{
		Volt::FramebufferSpecification spec{};

		spec.attachments =
		{
			{ Volt::ImageFormat::RGBA, { 0.1f, 0.1f, 0.1f, 1.f }  },
			{ Volt::ImageFormat::DEPTH32F }
		};

		spec.width = 1280;
		spec.height = 720;

		myParticlePass.framebuffer = Volt::Framebuffer::Create(spec);
		myParticlePass.debugName = "Particles";
	}
}

void ParticleEmitterEditor::UpdateMainContent()
{
	const ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable;
	if (ImGui::BeginTable("particleEditorMain", 2, tableFlags))
	{
		ImGui::TableSetupColumn("Viewport", 0, 250.f);
		ImGui::TableSetupColumn("Properties", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		UpdateViewport();
		ImGui::TableNextColumn();
		UpdateProperties();

		ImGui::EndTable();
	}
}

void ParticleEmitterEditor::OpenAsset(Ref<Volt::Asset> asset)
{
	OpenParticleSystem(asset->path);
}


bool ParticleEmitterEditor::SavePreset(const std::filesystem::path& indata)
{
	if (indata == "None" || !myCurrentPreset)
		return false;

	Volt::AssetManager::Get().SaveAsset(myCurrentPreset);
	return true;
}

void ParticleEmitterEditor::OnEvent(Volt::Event& e)
{
	Volt::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Volt::AppRenderEvent>(VT_BIND_EVENT_FN(ParticleEmitterEditor::OnRenderEvent));
	dispatcher.Dispatch<Volt::AppUpdateEvent>(VT_BIND_EVENT_FN(ParticleEmitterEditor::OnUpdateEvent));

	myCameraController->OnEvent(e);
}

bool ParticleEmitterEditor::OnRenderEvent(Volt::AppRenderEvent& e)
{
	Volt::Renderer::Begin();

	// Particles
	{
		Volt::Renderer::BeginPass(myParticlePass, myCameraController->GetCamera());

		if (myCurrentPreset)
		{
			for (int i = 0; i < myParticleSystemData.numberOfAliveParticles; i++)
			{
				auto& p = myParticleSystemData.particles[i];
				Volt::Renderer::SubmitBillboard(p.position, gem::vec3(0.001f), p.color, 0);
			}

			Ref<Volt::Shader> shader = Volt::ShaderRegistry::Get(myCurrentPreset->shader);
			if (shader != nullptr && shader->IsValid())
				Volt::Renderer::DispatchBillboardsWithShader(shader);
		}

		Volt::Renderer::SubmitSprite(gem::mat4{ 1.f }, { 1.f, 1.f, 1.f, 1.f });
		Volt::Renderer::DispatchSpritesWithShader(myGridShader);
		Volt::Renderer::EndPass();
	}

	Volt::Renderer::End();

	return false;
}

bool ParticleEmitterEditor::OnUpdateEvent(Volt::AppUpdateEvent& e)
{
	if (!myCurrentPreset)
	{
		return false;
	}

	myParticleSystemData.emittionTimer += e.GetTimestep();
	SendParticles(e.GetTimestep());
	auto& p_vec = myParticleSystemData.particles;
	for (int index = 0; index < myParticleSystemData.numberOfAliveParticles; index++)
	{
		// TODO: add updating stuff
		auto& p = p_vec[index];
		ParticlePositionUpdate(p, e.GetTimestep());
		if (ParticleKillCheck(p))
		{
			std::swap(p, p_vec[myParticleSystemData.numberOfAliveParticles - 1]);
			// -- ref
			myParticleSystemData.numberOfAliveParticles--;
		}
	}

	return false;
}

void ParticleEmitterEditor::UpdateViewport()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0.f, 0.f });
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2{ 0.f, 0.f });
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.f, 0.f });

	ImGui::BeginChild("viewport");
	myCameraController->SetIsControllable(ImGui::IsWindowHovered());

	auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
	auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
	auto viewportOffset = ImGui::GetWindowPos();

	myPerspectiveBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
	myPerspectiveBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	if (myViewportSize != (*(gem::vec2*)&viewportSize) && viewportSize.x > 0 && viewportSize.y > 0 && !Volt::Input::IsMouseButtonPressed(VT_MOUSE_BUTTON_LEFT))
	{
		myViewportSize = { viewportSize.x, viewportSize.y };
		myParticlePass.framebuffer->Resize((uint32_t)myViewportSize.x, (uint32_t)myViewportSize.y);
		myCameraController->UpdateProjection((uint32_t)myViewportSize.x, (uint32_t)myViewportSize.y);
	}

	ImGui::Image(UI::GetTextureID(myParticlePass.framebuffer->GetColorAttachment(0)), viewportSize);
	ImGui::EndChild();
	ImGui::PopStyleVar(3);
}

void ParticleEmitterEditor::UpdateProperties()
{
	ImGui::BeginChild("properties");

	myPresets.clear();
	myPresets.emplace_back("None");
	for (auto& a : Volt::AssetManager::Get().GetAssetRegistry())
	{
		if (Volt::AssetManager::Get().GetAssetTypeFromHandle(a.second) == Volt::AssetType::ParticlePreset)
		{
			myPresets.emplace_back(a.first.string());
		}
	}

	static int currentPresetSelected = 0;
	if (UI::Combo("Emitter Preset", currentPresetSelected, myPresets, 500))
	{
		if (currentPresetSelected > 0)
		{
			OpenParticleSystem(myPresets[currentPresetSelected]);
		}
	}

	if (!myCurrentPreset)
	{
		ImGui::EndChild();
		return;
	}

	UI::PushId();
	if (UI::BeginProperties())
	{
		UI::Property("Fade", myCurrentPreset->fade);
		UI::Property("Emissiveness", myCurrentPreset->emissiveness);
		UI::Property("Distance", myCurrentPreset->distance);
		UI::Property("Velocity", myCurrentPreset->velocity);
		UI::Property("Intensity", myCurrentPreset->intensity);
		UI::Property("Shader", myCurrentPreset->shader);
		UI::Property("Spread", myCurrentPreset->spread);
		UI::Property("Direction", myCurrentPreset->direction);
		UI::Property("Gravity", myCurrentPreset->gravity);
		UI::PropertyColor("Color", myCurrentPreset->color);
		UI::Property("Max Number Of Particles", myCurrentPreset->poolSize);
		UI::EndProperties();
		if (ImGui::Button("Save Data"))
		{
			SavePreset(myPresets[currentPresetSelected]);
		}
	}
	UI::PopId();

	ImGui::EndChild();
}

void ParticleEmitterEditor::SendParticles(float aDeltaTime)
{
	while (myParticleSystemData.emittionTimer > 1.0f / myCurrentPreset->intensity
		&& myParticleSystemData.numberOfAliveParticles < myParticleSystemData.particles.size())
	{
		auto& p = myParticleSystemData.particles[myParticleSystemData.numberOfAliveParticles];

		// TODO: make different starting patterns <func>
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution distrib(-myCurrentPreset->spread, myCurrentPreset->spread);
		gem::vec3 spread;

		spread = { distrib(gen), distrib(gen), distrib(gen) }; spread += myCurrentPreset->direction;
		while (spread.x == 0 && spread.y == 0 && spread.z == 0)
		{
			spread = { distrib(gen), distrib(gen), distrib(gen) }; spread += myCurrentPreset->direction;
		}

		p.startVelocity = myCurrentPreset->velocity;
		p.velocity = p.startVelocity;
		p.emissiveness = myCurrentPreset->emissiveness;
		p.fade = myCurrentPreset->fade;
		p.dead = false;
		p.direction = gem::normalize(spread);
		p.distance = 0;
		p.endDistance = myCurrentPreset->distance;
		// temp	value  <--------->
		p.startColor = myCurrentPreset->color;
		p.color = p.startColor;
		p.position = { 0.f, 0.f, 0.f };
		p.gravity = myCurrentPreset->gravity;
		myParticleSystemData.numberOfAliveParticles++;
		myParticleSystemData.emittionTimer -= 1.0f / myCurrentPreset->intensity;
	}
}

void ParticleEmitterEditor::ParticlePositionUpdate(Volt::Particle& aParticle, float aDeltaTime)
{
	aParticle.position += aParticle.velocity * aParticle.direction * aDeltaTime;
	aParticle.distance += aParticle.velocity * aDeltaTime;
	aParticle.direction += aParticle.gravity * aDeltaTime;
	if (aParticle.fade)
		aParticle.color = { aParticle.color.x, aParticle.color.y, aParticle.color.z, aParticle.startColor.w - (aParticle.distance / aParticle.endDistance) };
}

bool ParticleEmitterEditor::ParticleKillCheck(Volt::Particle& aParticle)
{
	if (aParticle.distance > aParticle.endDistance && aParticle.endDistance > 0)
		aParticle.dead = true;
	return aParticle.dead;
}

void ParticleEmitterEditor::OpenParticleSystem(const std::filesystem::path& aPath)
{
	myCurrentPreset = Volt::AssetManager::GetAsset<Volt::ParticlePreset>(aPath);
	myParticleSystemData = {};

	for (auto& p : myParticleSystemData.particles)
		p.dead = true;

	myParticleSystemData.particles.resize(myCurrentPreset->poolSize);
	myParticleSystemData.emittionTimer = 0;
	myParticleSystemData.numberOfAliveParticles = 0;
}

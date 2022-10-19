#include "sbpch.h"
#include "RendererSettingsPanel.h"

#include <Volt/Utility/UIUtility.h>
#include <Volt/Rendering/Renderer.h>

RendererSettingsPanel::RendererSettingsPanel()
	: EditorWindow("Renderer Settings")
{}

void RendererSettingsPanel::UpdateMainContent()
{
	UI::PushId();
	ImGui::TextUnformatted("Environment");
	if (UI::BeginProperties("env"))
	{
		UI::Property("Ambiance multiplier", Volt::Renderer::GetSettings().ambianceMultiplier);
		UI::EndProperties();
	}

	ImGui::Separator();
	ImGui::TextUnformatted("Camera");
	if (UI::BeginProperties("cam"))
	{
		UI::Property("Exposure", Volt::Renderer::GetSettings().exposure);
		UI::EndProperties();
	}

	UI::PopId();
}

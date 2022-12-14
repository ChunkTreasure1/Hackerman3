#include "sbpch.h"
#include "EditorSettingsPanel.h"

#include "Sandbox/Sandbox.h"
#include "Sandbox/VersionControl/VersionControl.h"

#include <Volt/Utility/UIUtility.h>
#include <imgui_stdlib.h>

EditorSettingsPanel::EditorSettingsPanel(EditorSettings& settings)
	: EditorWindow("Editor Settings"), m_editorSettings(settings)
{
	myWindowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
}

void EditorSettingsPanel::UpdateMainContent()
{
	const ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable;

	if (ImGui::BeginTable("settingsMain", 2, tableFlags))
	{
		ImGui::TableSetupColumn("Outline", 0, 250.f);
		ImGui::TableSetupColumn("View", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		DrawOutline();

		ImGui::TableNextColumn();

		DrawView();

		ImGui::EndTable();
	}
}

void EditorSettingsPanel::DrawOutline()
{
	ImGuiStyle& style = ImGui::GetStyle();
	auto color = style.Colors[ImGuiCol_FrameBg];
	UI::ScopedColor newColor(ImGuiCol_ChildBg, { color.x, color.y, color.z, color.w });

	ImGui::BeginChild("##outline");

	UI::ShiftCursor(5.f, 5.f);
	if (ImGui::Selectable("Version Control"))
	{
		m_currentMenu = SettingsMenu::VersionControl;
	}

	ImGui::EndChild();
}

void EditorSettingsPanel::DrawView()
{
	ImGui::BeginChild("##view", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetWindowHeight()));
	{
		ImGui::BeginChild("scrolling");

		switch (m_currentMenu)
		{
			case SettingsMenu::VersionControl: DrawVersionControl(); break;
			default: break;
		}

		ImGui::EndChild();
	}
	ImGui::EndChild();
}

void EditorSettingsPanel::DrawVersionControl()
{
	auto& versionControlSettings = m_editorSettings.versionControlSettings;

	UI::PushId();
	if (UI::BeginProperties())
	{
		UI::Property("Host", versionControlSettings.server);
		UI::Property("User", versionControlSettings.user);
		UI::Property("Password", versionControlSettings.password);

		UI::EndProperties();
	}

	{
		UI::ScopedColor buttonColor(ImGuiCol_Button, { 0.313f, 0.313f, 0.313f, 1.f });
		UI::ScopedStyleFloat buttonRounding(ImGuiStyleVar_FrameRounding, 2.f);

		if (ImGui::Button("Connect"))
		{
			if (!versionControlSettings.server.empty() &&
				!versionControlSettings.user.empty())
			{
				if (VersionControl::Connect(m_editorSettings.versionControlSettings.server, m_editorSettings.versionControlSettings.user, m_editorSettings.versionControlSettings.password))
				{
					VersionControl::RefreshStreams();
					VersionControl::RefreshWorkspaces();
				}
			}
		}
	}

	ImGui::Separator();
	ImGui::TextUnformatted("Settings");

	if (VersionControl::IsConnected())
	{
		std::vector<std::string> streams = VersionControl::GetStreams();
		if (streams.empty())
		{
			streams.emplace_back("Empty");
		}

		std::vector<std::string> workspaces = VersionControl::GetWorkspaces();
		if (workspaces.empty())
		{
			workspaces.emplace_back("Empty");
		}
 
		UI::PushId();
		if (UI::BeginProperties())
		{
			int32_t currentStream = m_currentStream;
			int32_t currentWorkspace = m_currentWorkspace;

			if (UI::ComboProperty("Workspace", m_currentWorkspace, workspaces))
			{
				if (currentWorkspace != m_currentWorkspace)
				{
					VersionControl::SwitchWorkspace(workspaces[m_currentWorkspace]);
					VersionControl::RefreshStreams();
				}
			}

			if (UI::ComboProperty("Stream", m_currentStream, streams))
			{
				if (currentStream != m_currentStream)
				{
					VersionControl::SwitchStream(streams[m_currentStream]);
				}
			}

			UI::EndProperties();
		}
		UI::PopId();

		ImGui::SameLine();
	}

	UI::PopId();
}

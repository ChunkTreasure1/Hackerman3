#pragma once

#include "Volt/Core/Base.h"
#include "Volt/Utility/FileSystem.h"

#include "Volt/Asset/AssetManager.h"
#include "Volt/Core/Application.h"

#include <gem/gem.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

#include <imgui_notify.h>

#include <xhash>
#include "Volt/Input/Input.h"
#include "Volt/Input/MouseButtonCodes.h"


namespace Volt
{
	class Texture2D;
	class Image2D;
}

enum class NotificationType
{
	Info,
	Warning,
	Error,
	Success
};

class UI
{
public:
	class ScopedColor
	{
	public:
		ScopedColor(ImGuiCol_ color, const gem::vec4& newColor)
			: m_Color(color)
		{
			auto& colors = ImGui::GetStyle().Colors;
			m_OldColor = colors[color];
			colors[color] = ImVec4{ newColor.x, newColor.y, newColor.z, newColor.w };
		}

		~ScopedColor()
		{
			auto& colors = ImGui::GetStyle().Colors;
			colors[m_Color] = m_OldColor;
		}

	private:
		ImVec4 m_OldColor;
		ImGuiCol_ m_Color;
	};

	class ScopedStyleFloat
	{
	public:
		ScopedStyleFloat(ImGuiStyleVar_ var, float value)
		{
			ImGui::PushStyleVar(var, value);
		}

		~ScopedStyleFloat()
		{
			ImGui::PopStyleVar();
		}
	};

	class ScopedStyleFloat2
	{
	public:
		ScopedStyleFloat2(ImGuiStyleVar_ var, const gem::vec2& value)
		{
			ImGui::PushStyleVar(var, { value.x, value.y });
		}

		~ScopedStyleFloat2()
		{
			ImGui::PopStyleVar();
		}
	};

	static ImTextureID GetTextureID(Ref<Volt::Texture2D> texture);
	static ImTextureID GetTextureID(Ref<Volt::Image2D> texture);
	static ImTextureID GetTextureID(Volt::Texture2D* texture);

	static ImGuiToastType ToastTypeFromNotificationType(NotificationType type)
	{
		switch (type)
		{
			case NotificationType::Info: return ImGuiToastType_Info;
			case NotificationType::Warning: return ImGuiToastType_Warning;
			case NotificationType::Error: return ImGuiToastType_Error;
			case NotificationType::Success: return ImGuiToastType_Success;
			default: return ImGuiToastType_None;
		}
	}

	static void ShiftCursor(float x, float y)
	{
		ImVec2 pos = { ImGui::GetCursorPosX() + x, ImGui::GetCursorPosY() + y };
		ImGui::SetCursorPos(pos);
	}

	static bool BeginPopup(const std::string& id = "", ImGuiPopupFlags flags = 0)
	{
		if (id.empty())
		{
			return ImGui::BeginPopupContextItem();
		}

		return ImGui::BeginPopupContextItem(id.c_str(), flags);
	}

	static bool BeginPopupWindow(const std::string& id = "")
	{
		if (id.empty())
		{
			return ImGui::BeginPopupContextWindow();
		}

		return ImGui::BeginPopupContextWindow(id.c_str());
	}

	static void EndPopup()
	{
		ImGui::EndPopup();
	}

	static bool InputText(const std::string& name, std::string& text, ImGuiInputTextFlags_ flags = ImGuiInputTextFlags_None)
	{
		if (!name.empty())
		{
			ImGui::TextUnformatted(name.c_str());
			ImGui::SameLine();
		}

		std::string id = "##" + std::to_string(s_stackId++);
		return ImGui::InputTextString(id.c_str(), &text, flags);
	}

	static bool InputTextMultiline(const std::string& name, std::string& text, ImGuiInputTextFlags_ flags = ImGuiInputTextFlags_None)
	{
		if (!name.empty())
		{
			ImGui::TextUnformatted(name.c_str());
			ImGui::SameLine();
		}

		std::string id = "##" + std::to_string(s_stackId++);
		return ImGui::InputTextMultilineString(id.c_str(), &text, ImVec2{ 0.f, 0.f }, flags);
	}

	static bool ImageButton(const std::string& id, ImTextureID textureId, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1))
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		if (window->SkipItems)
			return false;

		const ImGuiID imId = window->GetID(id.c_str());

		// Default to using texture ID as ID. User can still push string/integer prefixes.
		const ImVec2 padding = (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : g.Style.FramePadding;
		return ImGui::ImageButtonEx(imId, textureId, size, uv0, uv1, padding, bg_col, tint_col);
	}

	static bool ImageButtonState(const std::string& id, bool state, ImTextureID textureId, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1))
	{
		if (state)
		{
			return ImageButton(id, textureId, size, uv0, uv1, -1, { 0.18f, 0.18f, 0.18f, 1.f });
		}
		else
		{
			return ImageButton(id, textureId, size, uv0, uv1);
		}
	}

	static bool TreeNodeImage(Ref<Volt::Texture2D> texture, const std::string& text, ImGuiTreeNodeFlags flags)
	{
		ScopedStyleFloat2 frame{ ImGuiStyleVar_FramePadding, { 0.f, 0.f } };
		ScopedStyleFloat2 spacing{ ImGuiStyleVar_ItemSpacing, { 0.f, 0.f } };

		const ImVec2 size = ImGui::CalcTextSize(text.c_str());

		ImGui::Image(GetTextureID(texture), { size.y, size.y });
		ImGui::SameLine();

		return ImGui::TreeNodeEx(text.c_str(), flags);
	}

	static bool TreeNodeFramed(const std::string& text, bool alwaysOpen = false, bool useOther = false, float rounding = 0.f, const gem::vec2& padding = { 0.f, 0.f })
	{
		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

		if (alwaysOpen)
		{
			nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
		}

		if (!useOther)
		{
			return ImGui::TreeNodeEx(text.c_str(), nodeFlags);
		}

		UI::ScopedStyleFloat frameRound(ImGuiStyleVar_FrameRounding, rounding);

		return ImGui::TreeNodeEx(text.c_str(), nodeFlags);
	}

	static void SameLine(float offsetX = 0.f, float spacing = -1.f)
	{
		ImGui::SameLine(offsetX, spacing);
	}

	static bool ImageSelectable(Ref<Volt::Texture2D> texture, const std::string& text, bool& selected)
	{
		const ImVec2 size = ImGui::CalcTextSize(text.c_str());

		ImGui::Image(GetTextureID(texture), { size.y, size.y });
		ImGui::SameLine();

		return ImGui::Selectable(text.c_str(), &selected);
	}

	static bool ImageSelectable(Ref<Volt::Texture2D> texture, const std::string& text)
	{
		const ImVec2 size = ImGui::CalcTextSize(text.c_str());

		ImGui::Image(GetTextureID(texture), { size.y - 2.f, size.y - 2.f });
		ImGui::SameLine();

		UI::ShiftCursor(5.f, 0.f);

		return ImGui::Selectable(text.c_str());
	}

	static void TreeNodePop()
	{
		ImGui::TreePop();
	}

	static void PushId()
	{
		int id = s_contextId++;
		ImGui::PushID(id);
		s_stackId = 0;
	}

	static void PopId()
	{
		ImGui::PopID();
		s_contextId--;
	}

	static int32_t GetId()
	{
		return s_stackId++;
	}

	static bool IsInputEnabled()
	{
		const auto& io = ImGui::GetIO();
		return (io.ConfigFlags & ImGuiConfigFlags_NoMouse) == 0 && (io.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard) == 0;
	}

	static void SetInputEnabled(bool enable)
	{
		auto& io = ImGui::GetIO();

		if (enable)
		{
			io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
			io.ConfigFlags &= ~ImGuiConfigFlags_NavNoCaptureKeyboard;
		}
		else
		{
			io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
			io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;
		}
	}

	static bool BeginProperties(const std::string& name = "")
	{
		bool open = ImGui::BeginTable(name.c_str(), 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable);

		if (open)
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 0.3f);
			ImGui::TableSetupColumn("Properties", ImGuiTableColumnFlags_WidthStretch);
		}

		return open;
	}

	static void EndProperties()
	{
		ImGui::EndTable();
	}

	static bool ComboProperty(const std::string& text, int& currentItem, const std::vector<std::string>& strItems, float width = 0.f)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();

		if (width == 0.f)
		{
			ImGui::PushItemWidth(ImGui::GetColumnWidth());
		}
		else
		{
			ImGui::PushItemWidth(width);
		}
		std::string id = "##" + std::to_string(s_stackId++);

		std::vector<const char*> items;
		std::for_each(strItems.begin(), strItems.end(), [&](const std::string& string) { items.emplace_back(string.c_str()); });

		if (ImGui::Combo(id.c_str(), &currentItem, items.data(), (int32_t)items.size()))
		{
			changed = true;
		}

		ImGui::PopItemWidth();

		return changed;
	}

	static bool ComboProperty(const std::string& text, int& currentItem, const std::vector<const char*>& items, float width = 0.f)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();

		if (width == 0.f)
		{
			ImGui::PushItemWidth(ImGui::GetColumnWidth());
		}
		else
		{
			ImGui::PushItemWidth(width);
		}
		std::string id = "##" + std::to_string(s_stackId++);
		if (ImGui::Combo(id.c_str(), &currentItem, items.data(), (int32_t)items.size()))
		{
			changed = true;
		}

		ImGui::PopItemWidth();

		return changed;
	}

	static bool Combo(const std::string& text, int& currentItem, const std::vector<const char*>& items, float width = 100.f)
	{
		bool changed = false;

		ImGui::TextUnformatted(text.c_str());

		ImGui::SameLine();

		std::string id = "##" + std::to_string(s_stackId++);

		ImGui::SetNextItemWidth(width);
		if (ImGui::Combo(id.c_str(), &currentItem, items.data(), (int32_t)items.size()))
		{
			changed = true;
		}

		return changed;
	}

	static bool Combo(const std::string& text, int& currentItem, const std::vector<std::string>& strItems, float width = 100.f)
	{
		bool changed = false;

		ImGui::TextUnformatted(text.c_str());

		ImGui::SameLine();

		std::string id = "##" + std::to_string(s_stackId++);

		std::vector<const char*> items;
		std::for_each(strItems.begin(), strItems.end(), [&](const std::string& string) { items.emplace_back(string.c_str()); });

		ImGui::SetNextItemWidth(width);
		if (ImGui::Combo(id.c_str(), &currentItem, items.data(), (int32_t)items.size()))
		{
			changed = true;
		}

		return changed;
	}

	static void* DragDropTarget(const std::string& type)
	{
		void* data = nullptr;

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* pPayload = ImGui::AcceptDragDropPayload(type.c_str()))
			{
				data = pPayload->Data;
			}

			ImGui::EndDragDropTarget();
		}

		return data;
	}

	static void* DragDropTarget(std::initializer_list<std::string> types, ImGuiDragDropFlags flags = 0)
	{
		void* data = nullptr;

		for (const auto& type : types)
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* pPayload = ImGui::AcceptDragDropPayload(type.c_str(), flags))
				{
					data = pPayload->Data;
				}

				ImGui::EndDragDropTarget();
			}
		}

		return data;
	}

	static bool ImageSelectable(Ref<Volt::Texture2D> texture, const std::string& text, bool selected)
	{
		ImVec2 size = ImGui::CalcTextSize(text.c_str());
		ImGui::Image(GetTextureID(texture), { size.y, size.y }, { 0, 1 }, { 1, 0 });
		ImGui::SameLine();
		return ImGui::Selectable(text.c_str(), selected, ImGuiSelectableFlags_SpanAvailWidth);
	}

	static void Notify(NotificationType type, const std::string& title, const std::string& content, int32_t duration = 5000)
	{
		ImGuiToast toast{ ToastTypeFromNotificationType(type), duration };
		toast.set_title(title.c_str());
		toast.set_content(content.c_str());

		ImGui::InsertNotification(toast);
	}

	static void OpenModal(const std::string& name, ImGuiPopupFlags flags = 0)
	{
		const uint32_t nameHash = static_cast<uint32_t>(std::hash<std::string>()(name));
		ImGui::OpenPopupEx(nameHash, flags);
	}

	static bool BeginModal(const std::string& name, ImGuiWindowFlags flags = 0)
	{
		const uint32_t nameHash = static_cast<uint32_t>(std::hash<std::string>()(name));
		return ImGui::BeginPopupModal(name.c_str(), nameHash, nullptr, flags);
	}

	static void EndModal()
	{
		ImGui::EndPopup();
	}

	static bool AssetBrowserPopup(const std::string& id, Volt::AssetType wantedType, Volt::AssetHandle& assetHandle, std::function<void(Volt::AssetHandle& value)> callback = nullptr)
	{
		bool changed = false;

		if (ImGui::BeginPopup(id.c_str()))
		{
			for (const auto& [path, handle] : Volt::AssetManager::Get().GetAssetRegistry())
			{
				const Volt::AssetType assetType = Volt::AssetManager::Get().GetAssetTypeFromPath(path);
				if (wantedType == Volt::AssetType::None || wantedType == assetType)
				{
					std::string assetId = path.stem().string() + "###" + std::to_string(handle);
					if (ImGui::Selectable(assetId.c_str()))
					{
						assetHandle = handle;
						changed = true;
						if (callback)
						{
							callback(assetHandle);
						}
					}
				}
			}

			ImGui::EndPopup();
		}

		return changed;
	}

	static bool PropertyAxisColor(const std::string& text, gem::vec3& value, float resetValue = 0.f, std::function<void(gem::vec3& value)> callback = nullptr)
	{
		ScopedStyleFloat2 cellPad(ImGuiStyleVar_CellPadding, { 4.f, 0.f });

		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::Text(text.c_str());

		ImGui::TableNextColumn();
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.f, 0.f });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.f;
		ImVec2 buttonSize = { lineHeight + 3.f, lineHeight };

		{
			ScopedColor color{ ImGuiCol_Button, { 0.8f, 0.1f, 0.15f, 1.f } };
			ScopedColor colorh{ ImGuiCol_ButtonHovered, { 0.9f, 0.2f, 0.2f, 1.f } };
			ScopedColor colora{ ImGuiCol_ButtonActive, { 0.8f, 0.1f, 0.15f, 1.f } };

			std::string butId = "X##" + std::to_string(s_stackId++);
			if (ImGui::Button(butId.c_str(), buttonSize))
			{
				value.x = resetValue;
				changed = true;
				if (callback)
				{
					callback(value);
				}
			}
		}

		ImGui::SameLine();
		std::string id = "##" + std::to_string(s_stackId++);

		if (ImGui::DragFloat(id.c_str(), &value.x, 0.1f))
		{
			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		if (ImGui::IsItemHovered())
		{
			if (Volt::Input::IsMouseButtonPressed(VT_MOUSE_BUTTON_LEFT))
			{
				changed = true;
			}
		}

		ImGui::PopItemWidth();
		ImGui::SameLine();

		{
			ScopedColor color{ ImGuiCol_Button, { 0.2f, 0.7f, 0.2f, 1.f } };
			ScopedColor colorh{ ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.3f, 1.f } };
			ScopedColor colora{ ImGuiCol_ButtonActive, { 0.2f, 0.7f, 0.2f, 1.f } };

			std::string butId = "Y##" + std::to_string(s_stackId++);
			if (ImGui::Button(butId.c_str(), buttonSize))
			{
				value.y = resetValue;
				changed = true;
				if (callback)
				{
					callback(value);
				}
			}
		}

		ImGui::SameLine();
		id = "##" + std::to_string(s_stackId++);

		if (ImGui::DragFloat(id.c_str(), &value.y, 0.1f))
		{
			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		if (ImGui::IsItemHovered())
		{
			if (Volt::Input::IsMouseButtonPressed(VT_MOUSE_BUTTON_LEFT))
			{
				changed = true;
			}
		}

		ImGui::PopItemWidth();
		ImGui::SameLine();

		{
			ScopedColor color{ ImGuiCol_Button, { 0.1f, 0.25f, 0.8f, 1.f } };
			ScopedColor colorh{ ImGuiCol_ButtonHovered, { 0.2f, 0.35f, 0.9f, 1.f } };
			ScopedColor colora{ ImGuiCol_ButtonActive, { 0.1f, 0.25f, 0.8f, 1.f } };

			std::string butId = "Z##" + std::to_string(s_stackId++);
			if (ImGui::Button(butId.c_str(), buttonSize))
			{
				value.z = resetValue;
				changed = true;
				if (callback)
				{
					callback(value);
				}
			}
		}

		ImGui::SameLine();
		id = "##" + std::to_string(s_stackId++);
		if (ImGui::DragFloat(id.c_str(), &value.z, 0.1f))
		{
			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		if (ImGui::IsItemHovered())
		{
			if (Volt::Input::IsMouseButtonPressed(VT_MOUSE_BUTTON_LEFT))
			{
				changed = true;
			}
		}

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();

		return changed;
	}

	static bool Property(const std::string& text, bool& value, std::function<void(bool& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		std::string id = "##" + std::to_string(s_stackId++);

		if (ImGui::Checkbox(id.c_str(), &value))
		{
			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		return changed;
	}

	static bool Property(const std::string& text, int32_t& value, std::function<void(int32_t& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		std::string id = "##" + std::to_string(s_stackId++);
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (ImGui::DragScalar(id.c_str(), ImGuiDataType_S32, (void*)&value, 1.f))
		{
			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		ImGui::PopItemWidth();

		return changed;
	}

	static bool Property(const std::string& text, uint32_t& value, std::function<void(uint32_t& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		std::string id = "##" + std::to_string(s_stackId++);
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (ImGui::DragScalar(id.c_str(), ImGuiDataType_U32, (void*)&value, 1.f))
		{
			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		ImGui::PopItemWidth();

		return changed;
	}

	static bool Property(const std::string& text, int16_t& value, std::function<void(int16_t& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		std::string id = "##" + std::to_string(s_stackId++);
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (ImGui::DragScalar(id.c_str(), ImGuiDataType_S16, (void*)&value, 1.f))
		{
			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		ImGui::PopItemWidth();

		return changed;
	}

	static bool Property(const std::string& text, uint16_t& value, std::function<void(uint16_t& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		std::string id = "##" + std::to_string(s_stackId++);
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (ImGui::DragScalar(id.c_str(), ImGuiDataType_U16, (void*)&value, 1.f))
		{
			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		ImGui::PopItemWidth();

		return changed;
	}

	static bool Property(const std::string& text, int8_t& value, std::function<void(int8_t& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		std::string id = "##" + std::to_string(s_stackId++);
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (ImGui::DragScalar(id.c_str(), ImGuiDataType_S8, (void*)&value, 1.f))
		{
			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		ImGui::PopItemWidth();

		return changed;
	}

	static bool Property(const std::string& text, uint8_t& value, std::function<void(uint8_t& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		std::string id = "##" + std::to_string(s_stackId++);
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (ImGui::DragScalar(id.c_str(), ImGuiDataType_U8, (void*)&value, 1.f))
		{
			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		ImGui::PopItemWidth();

		return changed;
	}

	static bool Property(const std::string& text, double& value, std::function<void(double& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		std::string id = "##" + std::to_string(s_stackId++);
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (ImGui::DragScalar(id.c_str(), ImGuiDataType_Double, (void*)&value, 1.f))
		{
			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		ImGui::PopItemWidth();

		return changed;
	}

	static bool Property(const std::string& text, float& value, bool useMinMax = false, float min = 0.f, float max = 0.f, std::function<void(float& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		std::string id = "##" + std::to_string(s_stackId++);
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (ImGui::DragFloat(id.c_str(), &value, 1.f, min, max))
		{
			if (value < min && useMinMax)
			{
				value = min;
			}

			if (value > max && useMinMax)
			{
				value = max;
			}

			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		ImGui::PopItemWidth();

		return changed;
	}

	static bool Property(const std::string& text, gem::vec2& value, float min = 0.f, float max = 0.f, std::function<void(gem::vec2& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		std::string id = "##" + std::to_string(s_stackId++);
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (ImGui::DragFloat2(id.c_str(), gem::value_ptr(value), 1.f, min, max))
		{
			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		ImGui::PopItemWidth();

		return changed;
	}

	static bool Property(const std::string& text, gem::vec3& value, float min = 0.f, float max = 0.f, std::function<void(gem::vec3& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		std::string id = "##" + std::to_string(s_stackId++);
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (ImGui::DragFloat3(id.c_str(), gem::value_ptr(value), 1.f, min, max))
		{
			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		ImGui::PopItemWidth();

		return changed;
	}

	static bool Property(const std::string& text, gem::vec4& value, float min = 0.f, float max = 0.f, std::function<void(gem::vec4& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		std::string id = "##" + std::to_string(s_stackId++);
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (ImGui::DragFloat4(id.c_str(), gem::value_ptr(value), 1.f, min, max))
		{
			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		ImGui::PopItemWidth();

		return changed;
	}

	static bool Property(const std::string& text, gem::vec2ui& value, uint32_t min = 0, uint32_t max = 0, std::function<void(gem::vec2ui& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		std::string id = "##" + std::to_string(s_stackId++);
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (ImGui::DragScalarN(id.c_str(), ImGuiDataType_U32, gem::value_ptr(value), 2, 1.f, &min, &max))
		{
			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		ImGui::PopItemWidth();

		return changed;
	}

	static bool Property(const std::string& text, gem::vec3ui& value, uint32_t min = 0, uint32_t max = 0, std::function<void(gem::vec3ui& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		std::string id = "##" + std::to_string(s_stackId++);
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (ImGui::DragScalarN(id.c_str(), ImGuiDataType_U32, gem::value_ptr(value), 3, 1.f, &min, &max))
		{
			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		ImGui::PopItemWidth();

		return changed;
	}

	static bool Property(const std::string& text, gem::vec4ui& value, uint32_t min = 0, uint32_t max = 0, std::function<void(gem::vec4ui& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		std::string id = "##" + std::to_string(s_stackId++);
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (ImGui::DragScalarN(id.c_str(), ImGuiDataType_U32, gem::value_ptr(value), 4, 1.f, &min, &max))
		{
			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		ImGui::PopItemWidth();

		return changed;
	}

	static bool Property(const std::string& text, const std::string& value, bool readOnly = false, std::function<void(const std::string& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (InputText("", const_cast<std::string&>(value), readOnly ? ImGuiInputTextFlags_ReadOnly : ImGuiInputTextFlags_None))
		{
			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		ImGui::PopItemWidth();

		return changed;
	}

	static bool PropertyTextBox(const std::string& text, const std::string& value, bool readOnly = false)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (InputText("", const_cast<std::string&>(value), readOnly ? ImGuiInputTextFlags_ReadOnly : ImGuiInputTextFlags_None))
		{
			changed = true;
		}

		ImGui::PopItemWidth();

		return changed;
	}

	static bool Property(const std::string& text, std::string& value, bool readOnly = false, std::function<void(std::string& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (InputText("", value, readOnly ? ImGuiInputTextFlags_ReadOnly : ImGuiInputTextFlags_None))
		{
			changed = true;
			if (callback)
			{
				callback(value);
			}
		}

		return changed;
	}

	static bool PropertyMultiline(const std::string& text, std::string& value, bool readOnly = false)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (InputTextMultiline("", value, readOnly ? ImGuiInputTextFlags_ReadOnly : ImGuiInputTextFlags_None))
		{
			changed = true;
		}

		return changed;
	}

	static bool PropertyColor(const std::string& text, gem::vec4& value, std::function<void(gem::vec4& value)> callback = nullptr)
	{
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		std::string id = "##" + std::to_string(s_stackId++);
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (ImGui::ColorEdit4(id.c_str(), gem::value_ptr(value)))
		{
			if (callback)
			{
				callback(value);
			}
			return true;
		}

		return false;
	}

	static bool PropertyColor(const std::string& text, gem::vec3& value, std::function<void(gem::vec3& value)> callback = nullptr)
	{
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		std::string id = "##" + std::to_string(s_stackId++);
		ImGui::PushItemWidth(ImGui::GetColumnWidth());

		if (ImGui::ColorEdit3(id.c_str(), gem::value_ptr(value)))
		{
			if (callback)
			{
				callback(value);
			}
			return true;
		}

		return false;
	}

	static bool Property(const std::string& text, std::filesystem::path& path, std::function<void(std::filesystem::path& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		std::string sPath = path.string();
		ImGui::PushItemWidth(ImGui::GetColumnWidth() - ImGui::CalcTextSize("Open...").x - 20.f);

		if (InputText("", sPath))
		{
			path = std::filesystem::path(sPath);
			changed = true;
			if (callback)
			{
				callback(path);
			}
		}

		ImGui::PopItemWidth();
		ImGui::SameLine();

		std::string buttonId = "Open...##" + std::to_string(s_stackId++);
		if (ImGui::Button(buttonId.c_str(), { ImGui::GetContentRegionAvail().x, 25.f }))
		{
			auto newPath = FileSystem::OpenFile("All (*.*)\0*.*\0");
			if (!newPath.empty())
			{
				path = newPath;
				changed = true;
				if (callback)
				{
					callback(path);
				}
			}
		}

		//if (auto ptr = UI::DragDropTarget("CONTENT_BROWSER_ITEM"))
		//{
		//	const wchar_t* inPath = (const wchar_t*)ptr;
		//	std::filesystem::path newPath = std::filesystem::path("assets") / inPath;

		//	path = newPath;
		//	changed = true;
		//}

		return changed;
	}

	static bool PropertyDirectory(const std::string& text, std::filesystem::path& path, std::function<void(std::filesystem::path& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();
		std::string sPath = path.string();
		ImGui::PushItemWidth(ImGui::GetColumnWidth() - ImGui::CalcTextSize("Open...").x - 20.f);

		if (InputText("", sPath))
		{
			path = std::filesystem::path(sPath);
			changed = true;
			if (callback)
			{
				callback(path);
			}
		}

		ImGui::PopItemWidth();
		ImGui::SameLine();

		std::string buttonId = "Open...##" + std::to_string(s_stackId++);
		if (ImGui::Button(buttonId.c_str(), { ImGui::GetContentRegionAvail().x, 25.f }))
		{
			auto newPath = FileSystem::OpenFolder();
			if (!newPath.empty())
			{
				path = newPath;
				changed = true;
				if (callback)
				{
					callback(path);
				}
			}
		}

		//if (auto ptr = UI::DragDropTarget("CONTENT_BROWSER_ITEM"))
		//{
		//	const wchar_t* inPath = (const wchar_t*)ptr;
		//	std::filesystem::path newPath = std::filesystem::path("assets") / inPath;

		//	path = newPath;
		//	changed = true;
		//}

		return changed;
	}

	static bool Property(const std::string& text, Volt::AssetHandle& assetHandle, Volt::AssetType wantedType = Volt::AssetType::None, std::function<void(Volt::AssetHandle& value)> callback = nullptr)
	{
		bool changed = false;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.f, 0.f });

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();

		ImGui::PushItemWidth(ImGui::GetColumnWidth() - 2.f * 25.f);

		std::string assetFileName = "Null";

		const Ref<Volt::Asset> rawAsset = Volt::AssetManager::Get().GetAssetRaw(assetHandle);
		if (rawAsset)
		{
			assetFileName = rawAsset->path.filename().string();
		}

		std::string textId = "##" + std::to_string(s_stackId++);
		ImGui::InputTextString(textId.c_str(), &assetFileName, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();

		if (auto ptr = UI::DragDropTarget("ASSET_BROWSER_ITEM"))
		{
			Volt::AssetHandle newHandle = *(Volt::AssetHandle*)ptr;
			assetHandle = newHandle;
			changed = true;
			if (callback)
			{
				callback(assetHandle);
			}
		}

		ImGui::SameLine();

		std::string buttonId = "X##" + std::to_string(s_stackId++);
		if (ImGui::Button(buttonId.c_str(), { 24.5f, 24.5f }))
		{
			assetHandle = Volt::Asset::Null();
			changed = true;
			if (callback)
			{
				callback(assetHandle);
			}
		}

		ImGui::SameLine();

		std::string selectButtonId = "...##" + std::to_string(s_stackId++);
		std::string popupId = "AssetsPopup" + std::to_string(s_stackId);
		if (ImGui::Button(selectButtonId.c_str(), { 24.5f, 24.5f }))
		{
			ImGui::OpenPopup(popupId.c_str());
		}

		if (AssetBrowserPopup(popupId, wantedType, assetHandle, callback))
		{
			changed = true;
		}

		ImGui::PopStyleVar();

		return changed;
	}

	template<typename T>
	static bool Property(const std::string& text, Ref<T>& asset)
	{
		bool changed = false;

		ImGui::TableNextColumn();
		ImGui::TextUnformatted(text.c_str());

		ImGui::TableNextColumn();

		ImGui::PushItemWidth(ImGui::GetColumnWidth() - 20.f);

		std::string assetFileName = "Null";

		if (asset)
		{
			assetFileName = asset->path.filename().string();
		}

		std::string textId = "##" + std::to_string(s_stackId++);
		ImGui::InputTextString(textId.c_str(), &assetFileName, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();

		if (auto ptr = UI::DragDropTarget("ASSET_BROWSER_ITEM"))
		{
			Volt::AssetHandle newHandle = *(Volt::AssetHandle*)ptr;
			Ref<T> newAsset = Volt::AssetManager::GetAsset<T>(newHandle);
			if (newAsset)
			{
				asset = newAsset;
			}
			else
			{
				asset = nullptr;
			}

			changed = true;
		}

		ImGui::SameLine();

		std::string buttonId = "X##" + std::to_string(s_stackId++);
		if (ImGui::Button(buttonId.c_str()))
		{
			asset = nullptr;
			changed = true;
		}

		return changed;
	}

private:

	inline static uint32_t s_contextId = 0;
	inline static uint32_t s_stackId = 0;
};

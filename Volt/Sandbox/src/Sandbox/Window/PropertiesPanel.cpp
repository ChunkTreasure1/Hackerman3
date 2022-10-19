#include "sbpch.h"
#include "PropertiesPanel.h"

#include "Sandbox/Utility/SelectionManager.h"

#include <Volt/Utility/UIUtility.h>
#include <Volt/Scripting/ScriptRegistry.h>
#include <Volt/Scripting/ScriptEngine.h>
#include <Volt/Scripting/ScriptBase.h>

#include <Volt/Input/KeyCodes.h>
#include <Volt/Input/MouseButtonCodes.h>
#include <Volt/Input/Input.h>

#include <Wire/Serialization.h>

#include <vector>

#include "Sandbox/EditorCommandStack.h"

PropertiesPanel::PropertiesPanel(Ref<Volt::Scene>& currentScene)
	: EditorWindow("Properties"), myCurrentScene(currentScene)
{
	myIsOpen = true;
	myMaxEventListSize = 20;
	myLastValue = std::make_shared<PropertyEvent>();
}

void PropertiesPanel::UpdateMainContent()
{
	if (myMidEvent == true)
	{
		if (Volt::Input::IsMouseButtonReleased(VT_MOUSE_BUTTON_LEFT))
		{

			myMidEvent = false;
		}
	}

	UI::ScopedStyleFloat rounding{ ImGuiStyleVar_FrameRounding, 2.f };

	if (!SelectionManager::IsAnySelected())
	{
		return;
	}

	const bool singleSelected = !(SelectionManager::GetSelectedCount() > 1);
	auto& registry = myCurrentScene->GetRegistry();
	const auto firstEntity = SelectionManager::GetSelectedEntities().front();
	const auto entities = SelectionManager::GetSelectedEntities();

	if (singleSelected)
	{
		if (registry.HasComponent<Volt::TagComponent>(firstEntity))
		{
			auto& tag = registry.GetComponent<Volt::TagComponent>(firstEntity);
			UI::InputText("Name", tag.tag);
		}
	}
	else
	{
		static std::string inputText = "...";

		std::string firstName;
		bool sameName = true;

		if (registry.HasComponent<Volt::TagComponent>(firstEntity))
		{
			firstName = registry.GetComponent<Volt::TagComponent>(firstEntity).tag;
		}

		for (auto& entity : SelectionManager::GetSelectedEntities())
		{
			if (registry.HasComponent<Volt::TagComponent>(entity))
			{
				auto& tag = registry.GetComponent<Volt::TagComponent>(entity);
				if (tag.tag != firstName)
				{
					sameName = false;
					break;
				}
			}
		}

		if (sameName)
		{
			inputText = firstName;
		}
		else
		{
			inputText = "...";
		}

		UI::PushId();
		if (UI::InputText("Name", inputText))
		{
			for (auto& entity : SelectionManager::GetSelectedEntities())
			{
				if (registry.HasComponent<Volt::TagComponent>(entity))
				{
					registry.GetComponent<Volt::TagComponent>(entity).tag = inputText;
				}
			}
		}
		UI::PopId();
	}

	// Transform
	{
		UI::PushId();
		if (UI::BeginProperties("Transform"))
		{
			auto& entity = SelectionManager::GetSelectedEntities().front();

			if (registry.HasComponent<Volt::TransformComponent>(entity))
			{
				auto& transform = registry.GetComponent<Volt::TransformComponent>(entity); // #SAMUEL_TODO: Currently this displays local space if parented.

				if (UI::PropertyAxisColor("Position", transform.position, 0.f, (singleSelected) ? std::function<void(gem::vec3&)>() : [&](gem::vec3& val)
					{
						for (auto& ent : entities)
						{
							auto& entTransform = registry.GetComponent<Volt::TransformComponent>(ent);
							entTransform.position = val;
						}
					}))
				{
					if (myMidEvent == false)
					{
						Ref<ValueCommand<gem::vec3>> command = CreateRef<ValueCommand<gem::vec3>>(&transform.position, transform.position);
						EditorCommandStack::PushUndo(command);
						myMidEvent = true;
					}
				}

					gem::vec3 rotDegrees = gem::degrees(transform.rotation);
					if (UI::PropertyAxisColor("Rotation", rotDegrees, 0.f, (singleSelected) ? std::function<void(gem::vec3&)>() : [&](gem::vec3& val)
						{
							for (auto& ent : entities)
							{
								auto& entTransform = registry.GetComponent<Volt::TransformComponent>(ent);
								entTransform.rotation = gem::radians(val);
							}
						}))
					{
						transform.rotation = gem::radians(rotDegrees);

						if (myMidEvent == false)
						{
							Ref<ValueCommand<gem::vec3>> command = CreateRef<ValueCommand<gem::vec3>>(&transform.rotation, transform.rotation);
							EditorCommandStack::PushUndo(command);
							myMidEvent = true;
						}
					}

						if (UI::PropertyAxisColor("Scale", transform.scale, 1.f, (singleSelected) ? std::function<void(gem::vec3&)>() : [&](gem::vec3& val)
							{
								for (auto& ent : entities)
								{
									auto& entTransform = registry.GetComponent<Volt::TransformComponent>(ent);
									entTransform.scale = val;
								}
							}))
						{
							if (myMidEvent == false)
							{
								Ref<ValueCommand<gem::vec3>> command = CreateRef<ValueCommand<gem::vec3>>(&transform.scale, transform.scale);
								EditorCommandStack::PushUndo(command);
								myMidEvent = true;
							}
						}
			}

			UI::EndProperties();
		}
		UI::PopId();
	}

	if (singleSelected)
	{
		const auto entity = SelectionManager::GetSelectedEntities().front();

		for (const auto& [guid, pool] : registry.GetPools())
		{
			if (!registry.HasComponent(guid, entity))
			{
				continue;
			}

			const auto& registryInfo = Wire::ComponentRegistry::GetRegistryDataFromGUID(guid);
			if (registryInfo.name == "TagComponent" || registryInfo.name == "TransformComponent" || registryInfo.name == "RelationshipComponent" || registryInfo.name == "PrefabComponent" || 
				registryInfo.name == "EntityDataComponent")
			{
				continue;
			}

			bool removeComp = false;
			bool open = UI::TreeNodeFramed(registryInfo.name, true, false, 0.f, { 0.f, 2.f });
			float buttonSize = 21.f + GImGui->Style.FramePadding.y;
			float availRegion = ImGui::GetContentRegionAvail().x;

			if (!open)
			{
				UI::SameLine(availRegion - buttonSize * 0.5f + GImGui->Style.FramePadding.x * 0.5f);
			}
			else
			{
				UI::SameLine(availRegion + buttonSize * 0.5f + GImGui->Style.FramePadding.x * 0.5f);
			}
			std::string id = "-###Remove" + registryInfo.name;

			{
				UI::ScopedStyleFloat round{ ImGuiStyleVar_FrameRounding, 0.f };
				UI::ScopedStyleFloat2 pad{ ImGuiStyleVar_FramePadding, { 0.f, 0.f } };
				UI::ScopedColor color{ ImGuiCol_Button, { 0.8f, 0.1f, 0.15f, 1.f } };
				UI::ScopedColor colorh{ ImGuiCol_ButtonHovered, { 0.9f, 0.2f, 0.2f, 1.f } };
				UI::ScopedColor colora{ ImGuiCol_ButtonActive, { 0.8f, 0.1f, 0.15f, 1.f } };

				if (ImGui::Button(id.c_str(), ImVec2{ buttonSize, buttonSize }))
				{
					removeComp = true;
				}
			}

			if (open)
			{
				UI::PushId();
				if (registryInfo.name != "ScriptComponent" && UI::BeginProperties(registryInfo.name))
				{
					uint8_t* data = (uint8_t*)registry.GetComponentPtr(guid, entity);
					for (auto& prop : registryInfo.properties)
					{
						if (!prop.visible)
						{
							continue;
						}

						switch (prop.type)
						{
							case Wire::ComponentRegistry::PropertyType::Bool: UI::Property(prop.name, *(bool*)(&data[prop.offset])); break;
							case Wire::ComponentRegistry::PropertyType::String: UI::Property(prop.name, *(std::string*)(&data[prop.offset])); break;

							case Wire::ComponentRegistry::PropertyType::Int: UI::Property(prop.name, *(int32_t*)(&data[prop.offset])); break;
							case Wire::ComponentRegistry::PropertyType::UInt: UI::Property(prop.name, *(uint32_t*)(&data[prop.offset])); break;

							case Wire::ComponentRegistry::PropertyType::Short: UI::Property(prop.name, *(int16_t*)(&data[prop.offset])); break;
							case Wire::ComponentRegistry::PropertyType::UShort: UI::Property(prop.name, *(uint16_t*)(&data[prop.offset])); break;

							case Wire::ComponentRegistry::PropertyType::Char: UI::Property(prop.name, *(int8_t*)(&data[prop.offset])); break;
							case Wire::ComponentRegistry::PropertyType::UChar: UI::Property(prop.name, *(uint8_t*)(&data[prop.offset])); break;

							case Wire::ComponentRegistry::PropertyType::Float: UI::Property(prop.name, *(float*)(&data[prop.offset])); break;
							case Wire::ComponentRegistry::PropertyType::Double: UI::Property(prop.name, *(double*)(&data[prop.offset])); break;

							case Wire::ComponentRegistry::PropertyType::Vector2: UI::Property(prop.name, *(gem::vec2*)(&data[prop.offset])); break;
							case Wire::ComponentRegistry::PropertyType::Vector3: UI::Property(prop.name, *(gem::vec3*)(&data[prop.offset])); break;
							case Wire::ComponentRegistry::PropertyType::Vector4: UI::Property(prop.name, *(gem::vec4*)(&data[prop.offset])); break;

							case Wire::ComponentRegistry::PropertyType::Color3: UI::PropertyColor(prop.name, *(gem::vec3*)(&data[prop.offset])); break;
							case Wire::ComponentRegistry::PropertyType::Color4: UI::PropertyColor(prop.name, *(gem::vec4*)(&data[prop.offset])); break;

							case Wire::ComponentRegistry::PropertyType::AssetHandle: UI::Property(prop.name, *(Volt::AssetHandle*)(&data[prop.offset])); break;
							case Wire::ComponentRegistry::PropertyType::Folder: UI::PropertyDirectory(prop.name, *(std::filesystem::path*)(&data[prop.offset])); break;
							case Wire::ComponentRegistry::PropertyType::Path: UI::Property(prop.name, *(std::filesystem::path*)(&data[prop.offset])); break;
						
							case Wire::ComponentRegistry::PropertyType::Enum:
							{
								auto& enumData = Wire::ComponentRegistry::EnumData();
								if (enumData.find(prop.enumName) != enumData.end())
								{
									UI::ComboProperty(prop.name, *(int32_t*)(&data[prop.offset]), enumData.at(prop.enumName));
								}
								break;
							}
						}
					}

					UI::EndProperties();
				}
				else if (registryInfo.name == "ScriptComponent")
				{
					Volt::ScriptComponent& scriptComp = registry.GetComponent<Volt::ScriptComponent>(entity);
					if (UI::BeginProperties("scriptComponent"))
					{
						for (const auto& script : scriptComp.scripts)
						{
							std::string name = Volt::ScriptRegistry::GetNameFromGUID(script);
							UI::Property("Name", name);
						}
						UI::EndProperties();
					}

				}
				UI::PopId();

				UI::TreeNodePop();
			}

			if (removeComp)
			{
				myCurrentScene->GetRegistry().RemoveComponent(registryInfo.guid, entity);
			}
		}
	}
	else
	{
		auto componentsInCommon = registry.GetPools();

		for (auto& entity : entities)
		{
			for (const auto& [guid, pool] : registry.GetPools())
			{
				if (!registry.HasComponent(guid, entity))
				{
					componentsInCommon.erase(guid);
					continue;
				}
			}
		}

		const auto& entity = firstEntity;

		for (const auto& [guid, pool] : componentsInCommon)
		{
			if (!registry.HasComponent(guid, entity))
			{
				continue;
			}

			const auto& registryInfo = Wire::ComponentRegistry::GetRegistryDataFromGUID(guid);
			if (registryInfo.name == "TagComponent" || registryInfo.name == "TransformComponent" || registryInfo.name == "RelationshipComponent" || registryInfo.name == "PrefabComponent" ||
				registryInfo.name == "EntityDataComponent")
			{
				continue;
			}

			bool removeComp = false;
			bool open = UI::TreeNodeFramed(registryInfo.name.c_str(), true, false, 0.f, { 0.f, 2.f });
			float buttonSize = 21.f + GImGui->Style.FramePadding.y;
			float availRegion = ImGui::GetContentRegionAvail().x;

			if (!open)
			{
				UI::SameLine(availRegion - buttonSize * 0.5f + GImGui->Style.FramePadding.x * 0.5f);
			}
			else
			{
				UI::SameLine(availRegion + buttonSize * 0.5f + GImGui->Style.FramePadding.x * 0.5f);
			}
			std::string id = "-###Remove" + registryInfo.name;

			{
				UI::ScopedStyleFloat round{ ImGuiStyleVar_FrameRounding, 0.f };
				UI::ScopedStyleFloat2 pad{ ImGuiStyleVar_FramePadding, { 0.f, 0.f } };
				UI::ScopedColor color{ ImGuiCol_Button, { 0.8f, 0.1f, 0.15f, 1.f } };
				UI::ScopedColor colorh{ ImGuiCol_ButtonHovered, { 0.9f, 0.2f, 0.2f, 1.f } };
				UI::ScopedColor colora{ ImGuiCol_ButtonActive, { 0.8f, 0.1f, 0.15f, 1.f } };

				if (ImGui::Button(id.c_str(), ImVec2{ buttonSize, buttonSize }))
				{
					removeComp = true;
				}
			}

			if (open)
			{
				UI::PushId();
				if (registryInfo.name != "ScriptComponent" && UI::BeginProperties(registryInfo.name))
				{
					uint8_t* data = (uint8_t*)registry.GetComponentPtr(guid, entity);
					for (const auto& prop : registryInfo.properties)
					{
						if (!prop.visible)
						{
							continue;
						}

						switch (prop.type)
						{
							case Wire::ComponentRegistry::PropertyType::Bool: UI::Property(prop.name, *(bool*)(&data[prop.offset]), [&](bool& val)
								{
									for (auto& ent : entities)
									{
										uint8_t* entData = (uint8_t*)registry.GetComponentPtr(guid, ent);
										*(bool*)&entData[prop.offset] = val;
									}
								}); break;
							case Wire::ComponentRegistry::PropertyType::String: UI::Property(prop.name, *(std::string*)(&data[prop.offset]), false, [&](std::string& val)
								{
									for (auto& ent : entities)
									{
										uint8_t* entData = (uint8_t*)registry.GetComponentPtr(guid, ent);
										*(std::string*)&entData[prop.offset] = val;
									}
								}); break;

							case Wire::ComponentRegistry::PropertyType::Int: UI::Property(prop.name, *(int32_t*)(&data[prop.offset]), [&](int32_t& val)
								{
									for (auto& ent : entities)
									{
										uint8_t* entData = (uint8_t*)registry.GetComponentPtr(guid, ent);
										*(int32_t*)&entData[prop.offset] = val;
									}
								}); break;
							case Wire::ComponentRegistry::PropertyType::UInt: UI::Property(prop.name, *(uint32_t*)(&data[prop.offset]), [&](uint32_t& val)
								{
									for (auto& ent : entities)
									{
										uint8_t* entData = (uint8_t*)registry.GetComponentPtr(guid, ent);
										*(uint32_t*)&entData[prop.offset] = val;
									}
								}); break;

							case Wire::ComponentRegistry::PropertyType::Short: UI::Property(prop.name, *(int16_t*)(&data[prop.offset]), [&](int16_t& val)
								{
									for (auto& ent : entities)
									{
										uint8_t* entData = (uint8_t*)registry.GetComponentPtr(guid, ent);
										*(int16_t*)&entData[prop.offset] = val;
									}
								}); break;
							case Wire::ComponentRegistry::PropertyType::UShort: UI::Property(prop.name, *(uint16_t*)(&data[prop.offset]), [&](uint16_t& val)
								{
									for (auto& ent : entities)
									{
										uint8_t* entData = (uint8_t*)registry.GetComponentPtr(guid, ent);
										*(uint16_t*)&entData[prop.offset] = val;
									}
								}); break;

							case Wire::ComponentRegistry::PropertyType::Char: UI::Property(prop.name, *(int8_t*)(&data[prop.offset]), [&](int8_t& val)
								{
									for (auto& ent : entities)
									{
										uint8_t* entData = (uint8_t*)registry.GetComponentPtr(guid, ent);
										*(int8_t*)&entData[prop.offset] = val;
									}
								}); break;
							case Wire::ComponentRegistry::PropertyType::UChar: UI::Property(prop.name, *(uint8_t*)(&data[prop.offset]), [&](uint8_t& val)
								{
									for (auto& ent : entities)
									{
										uint8_t* entData = (uint8_t*)registry.GetComponentPtr(guid, ent);
										*(uint8_t*)&entData[prop.offset] = val;
									}
								}); break;

							case Wire::ComponentRegistry::PropertyType::Float: UI::Property(prop.name, *(float*)(&data[prop.offset]), false, 0.f, 0.f, [&](float& val)
								{
									for (auto& ent : entities)
									{
										uint8_t* entData = (uint8_t*)registry.GetComponentPtr(guid, ent);
										*(float*)&entData[prop.offset] = val;
									}
								}); break;
							case Wire::ComponentRegistry::PropertyType::Double: UI::Property(prop.name, *(double*)(&data[prop.offset]), [&](double& val)
								{
									for (auto& ent : entities)
									{
										uint8_t* entData = (uint8_t*)registry.GetComponentPtr(guid, ent);
										*(double*)&entData[prop.offset] = val;
									}
								}); break;

							case Wire::ComponentRegistry::PropertyType::Vector2: UI::Property(prop.name, *(gem::vec2*)(&data[prop.offset]), 0.f, 0.f, [&](gem::vec2& val)
								{
									for (auto& ent : entities)
									{
										uint8_t* entData = (uint8_t*)registry.GetComponentPtr(guid, ent);
										*(gem::vec2*)&entData[prop.offset] = val;
									}
								}); break;
							case Wire::ComponentRegistry::PropertyType::Vector3: UI::Property(prop.name, *(gem::vec3*)(&data[prop.offset]), 0.f, 0.f, [&](gem::vec3& val)
								{
									for (auto& ent : entities)
									{
										uint8_t* entData = (uint8_t*)registry.GetComponentPtr(guid, ent);
										*(gem::vec3*)&entData[prop.offset] = val;
									}
								}); break;
							case Wire::ComponentRegistry::PropertyType::Vector4: UI::Property(prop.name, *(gem::vec4*)(&data[prop.offset]), 0.f, 0.f, [&](gem::vec4& val)
								{
									for (auto& ent : entities)
									{
										uint8_t* entData = (uint8_t*)registry.GetComponentPtr(guid, ent);
										*(gem::vec4*)&entData[prop.offset] = val;
									}
								}); break;

							case Wire::ComponentRegistry::PropertyType::Color3: UI::PropertyColor(prop.name, *(gem::vec3*)(&data[prop.offset]), [&](gem::vec3& val)
								{
									for (auto& ent : entities)
									{
										uint8_t* entData = (uint8_t*)registry.GetComponentPtr(guid, ent);
										*(gem::vec3*)&entData[prop.offset] = val;
									}
								}); break;
							case Wire::ComponentRegistry::PropertyType::Color4: UI::PropertyColor(prop.name, *(gem::vec4*)(&data[prop.offset]), [&](gem::vec4& val)
								{
									for (auto& ent : entities)
									{
										uint8_t* entData = (uint8_t*)registry.GetComponentPtr(guid, ent);
										*(gem::vec4*)&entData[prop.offset] = val;
									}
								}); break;

							case Wire::ComponentRegistry::PropertyType::AssetHandle: UI::Property(prop.name, *(Volt::AssetHandle*)(&data[prop.offset]), Volt::AssetType::None, [&](Volt::AssetHandle& val)
								{
									for (auto& ent : entities)
									{
										uint8_t* entData = (uint8_t*)registry.GetComponentPtr(guid, ent);
										*(Volt::AssetHandle*)&entData[prop.offset] = val;
									}
								}); break;
							case Wire::ComponentRegistry::PropertyType::Folder: UI::PropertyDirectory(prop.name, *(std::filesystem::path*)(&data[prop.offset]), [&](std::filesystem::path& val)
								{
									for (auto& ent : entities)
									{
										uint8_t* entData = (uint8_t*)registry.GetComponentPtr(guid, ent);
										*(std::filesystem::path*)&entData[prop.offset] = val;
									}
								}); break;
							case Wire::ComponentRegistry::PropertyType::Path: UI::Property(prop.name, *(std::filesystem::path*)(&data[prop.offset]), [&](std::filesystem::path& val)
								{
									for (auto& ent : entities)
									{
										uint8_t* entData = (uint8_t*)registry.GetComponentPtr(guid, ent);
										*(std::filesystem::path*)&entData[prop.offset] = val;
									}
								}); break;

							case Wire::ComponentRegistry::PropertyType::Enum:
							{
								auto& enumData = Wire::ComponentRegistry::EnumData();
								if (enumData.find(prop.enumName) != enumData.end())
								{
									UI::ComboProperty(prop.name, *(int32_t*)(&data[prop.offset]), enumData.at(prop.enumName));
								}
								break;
							}
						}
					}

					UI::EndProperties();
				}
				UI::PopId();

				UI::TreeNodePop();
			}

			if (removeComp)
			{
				for (auto& ent : entities)
				{
					myCurrentScene->GetRegistry().RemoveComponent(registryInfo.guid, ent);
				}
			}
		}
	}

	ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.2f, 0.2f, 1.f });
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.f);
	if (ImGui::Button("Add Component"))
	{
		ImGui::OpenPopup("AddComponent");
	}
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();

	AddComponentPopup();
}

void PropertiesPanel::AddComponentPopup()
{
	ImGui::SetNextWindowSize({ 250.f, 500.f } );
	if (ImGui::BeginPopup("AddComponent"))
	{
		const auto& componentInfo = Wire::ComponentRegistry::ComponentGUIDs();
		for (const auto& [name, info] : componentInfo)
		{
			if (!myCurrentScene->GetRegistry().HasComponent(info.guid, SelectionManager::GetSelectedEntities().front()) && ImGui::MenuItem(name.c_str()))
			{
				for (auto& ent : SelectionManager::GetSelectedEntities())
				{
					if (!myCurrentScene->GetRegistry().HasComponent(info.guid, ent))
					{
						myCurrentScene->GetRegistry().AddComponent(info.guid, ent);
					}
				}

				ImGui::CloseCurrentPopup();
			}
		}

		if (ImGui::TreeNode("Scripts"))
		{
			const auto& scriptInfo = Volt::ScriptRegistry::GetRegistry();

			for (const auto& [GUID, info] : scriptInfo)
			{
				if (ImGui::MenuItem(info.name.c_str()))
				{
					for (auto& ent : SelectionManager::GetSelectedEntities())
					{
						if (!myCurrentScene->GetRegistry().HasComponent<Volt::ScriptComponent>(ent))
						{
							myCurrentScene->GetRegistry().AddComponent<Volt::ScriptComponent>(ent);
						}

						auto& comp = myCurrentScene->GetRegistry().GetComponent<Volt::ScriptComponent>(ent);
						if (std::find(comp.scripts.begin(), comp.scripts.end(), GUID) != comp.scripts.end())
						{
							continue;
						}

						comp.scripts.emplace_back(GUID);
					}
				}
			}

			ImGui::TreePop();
		}

		ImGui::EndPopup();
	}
}

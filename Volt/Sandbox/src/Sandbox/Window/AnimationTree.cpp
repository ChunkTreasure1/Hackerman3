#include "sbpch.h"
#include "AnimationTree.h"
#include <yaml-cpp/yaml.h>
#include "Volt/Utility/SerializationMacros.h"
#include "Volt/Utility/YAMLSerializationHelpers.h"
#include "Volt/Utility/FileSystem.h"

#include <Windows.h>

namespace ed = ax::NodeEditor;

AnimationTree::AnimationTree()
	: EditorWindow("AnimationTree")
{
	myWindowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	myNodePanelFlags = ImGuiWindowFlags_MenuBar;
	myIsOpen = false;



	s_Contexts.push_back(ed::CreateEditor(&myDefaultConfig));
	//s_Contexts.push_back(ed::CreateEditor(&myOtherDefaultConfig));
	ed::SetCurrentEditor(ed::CreateEditor(&myDefaultConfig));

	//Node* node;
	//node = SpawnTreeSequenceNode(GetNextId(), "FirstNode");			ed::SetNodePosition(node->nodeID, ImVec2(0, 0));
	//node = SpawnTreeSequenceNode(GetNextId(), "SecondNode");		ed::SetNodePosition(node->nodeID, ImVec2(100, 100));
	//node = SpawnTreeSequenceNode(GetNextId(), "ThirdNode");			ed::SetNodePosition(node->nodeID, ImVec2(-100, -100));

	//ed::NavigateToContent();

	//BuildAllNodes();
}

void AnimationTree::UpdateMainContent()
{
	auto& io = ImGui::GetIO();

	ShowLeftPane();
	ImGui::SameLine();
	{
		if (ImGui::SmallButton("Context0")) 
		{
			currentContext = 0;
		}
		if (ImGui::SmallButton("Context1"))
		{
			currentContext = 1;
		}

		static ImVec2 WindowSize = { 1024,1024 };
		ImGui::BeginChild("NODE EDITOR", WindowSize, true, 0);
		{
			ed::SetCurrentEditor(s_Contexts[currentContext]);
			ed::Begin("NodeEditor", ImVec2(0, 0));
			// Start drawing nodes.
			for (auto& node : s_Nodes)
			{
				const float rounding = 5.0f;
				const float padding = 5.0f;

				const auto pinBackground = ed::GetStyle().Colors[ed::StyleColor_NodeBg];

				ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(128, 128, 128, 200));
				ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(32, 32, 32, 200));
				ed::PushStyleColor(ed::StyleColor_PinRect, ImColor(60, 180, 255, 150));
				ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(60, 180, 255, 150));

				ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
				ed::PushStyleVar(ed::StyleVar_NodeRounding, rounding);
				ed::PushStyleVar(ed::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
				ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
				ed::PushStyleVar(ed::StyleVar_LinkStrength, 0.0f);
				ed::PushStyleVar(ed::StyleVar_PinBorderWidth, 1.0f);
				ed::PushStyleVar(ed::StyleVar_PinRadius, 10.0f);

				ed::BeginNode(node.nodeID);


				//Draw Inputs
				if (!node.inputs.empty())
				{
					auto& pin = node.inputs[0];
					ImGui::Dummy(ImVec2(0, padding));

					ed::PushStyleVar(ed::StyleVar_PinCorners, 12);

					ImGui::GetItemRectMin();

					ed::BeginPin(node.inputs[0].id, ed::PinKind::Input);
					ImGui::Text("Input");
					ed::PinPivotRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
					ed::PinRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
					ed::EndPin();
					ed::PopStyleVar();


				}
				ImGui::NewLine();
				ImGui::Text(node.name.c_str());
				ImGui::NewLine();
				//Draw Outputs
				if (!node.outputs.empty())
				{
					auto& pin = node.outputs[0];
					ImGui::Dummy(ImVec2(0, padding));
					ed::PushStyleVar(ed::StyleVar_PinArrowSize, 10.0f);
					ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 10.0f);
					ed::PushStyleVar(ed::StyleVar_PinCorners, 12);

					ed::BeginPin(pin.id, ed::PinKind::Output);
					ImGui::Text("Output");
					ed::EndPin();
					ed::PopStyleVar(3);
				}

				ed::EndNode();
				ed::PopStyleVar(7);
				ed::PopStyleColor(4);


			}

			for (auto& link : s_Links)
				ed::Link(link.linkID, link.startPin, link.endPin, link.color, 2.0f);


			static Pin* newLinkPin = nullptr;
			if (ed::BeginCreate(ImColor(255, 255, 0), 2.0f))
			{
				auto showLabel = [](const char* label, ImColor color)
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
					auto size = ImGui::CalcTextSize(label);

					auto padding = ImGui::GetStyle().FramePadding;
					auto spacing = ImGui::GetStyle().ItemSpacing;

					ImGui::SetCursorPos(ImGui::GetCursorPos()/* + ImVec2(spacing.x, -spacing.y)*/);

					//auto rectMin = ImGui::GetCursorScreenPos() - padding;
					//auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

					//auto rectMin = ImGui::GetCursorScreenPos() - 20;
					//auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

					auto drawList = ImGui::GetWindowDrawList();
					drawList->AddRectFilled(ImVec2(128, 128), ImVec2(128, 128), color, size.y * 0.15f);
					ImGui::TextUnformatted(label);
				};

				ed::PinId startPinId = 0, endPinId = 0;
				if (ed::QueryNewLink(&startPinId, &endPinId))
				{
					auto startPin = FindPin(startPinId);
					auto endPin = FindPin(endPinId);

					newLinkPin = startPin ? startPin : endPin;

					if (startPin->mode == PinMode::Input)
					{
						std::swap(startPin, endPin);
						std::swap(startPinId, endPinId);
					}

					if (startPin && endPin)
					{
						if (endPin == startPin)
						{
							ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
						}
						else if (endPin->mode == startPin->mode)
						{
							showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
							ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
						}
						else if (endPin->node == startPin->node)
						{
							showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
							ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
						}
						else if (endPin->type != startPin->type)
						{
							showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
							ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
						}
						else
						{
							showLabel("+ Create Link", ImColor(32, 45, 32, 180));
							if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
							{
								//s_Links.emplace_back(Link(GetNextId(), startPinId, endPinId));
								s_Links.emplace_back(Link(GetNextId(), startPin->id, endPin->id));
								//s_Links.back().Color = GetIconColor(startPin->Type);
								s_Links.back().color = ImColor(0, 0, 255);
							}
						}
					}
				}
			}

			ed::EndCreate();

			if (ed::BeginDelete())
			{
				ed::LinkId linkId = 0;
				while (ed::QueryDeletedLink((&linkId)))
				{
					if (ed::AcceptDeletedItem())
					{
						auto id = std::find_if(s_Links.begin(), s_Links.end(), [linkId](auto& link) { return link.linkID == linkId; });
						if (id != s_Links.end())
						{
							s_Links.erase(id);
						}

					}
				}

				ed::NodeId nodeId = 0;
				while (ed::QueryDeletedNode(&nodeId))
				{
					if (ed::AcceptDeletedItem())
					{
						auto id = std::find_if(s_Nodes.begin(), s_Nodes.end(), [nodeId](auto& node) { return node.nodeID == nodeId; });
						if (id != s_Nodes.end())
						{
							s_Nodes.erase(id);
						}
					}
				}
			}
			ed::EndDelete();

			auto openPopupPosition = ImGui::GetMousePos();
			ed::Suspend();
			if (ed::ShowBackgroundContextMenu())
			{
				ImGui::OpenPopup("Create New Node");
			}
			ed::Resume();

			ed::Suspend();
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
			if (ImGui::BeginPopup("Create New Node"))
			{
				auto newNodePostion = openPopupPosition;
				//ImGui::SetCursorScreenPos(ImGui::GetMousePosOnOpeningCurrentPopup());

				//auto drawList = ImGui::GetWindowDrawList();
				//drawList->AddCircleFilled(ImGui::GetMousePosOnOpeningCurrentPopup(), 10.0f, 0xFFFF00FF);

				Node* node = nullptr;

				if (ImGui::MenuItem("Create New Node"))
					node = SpawnTreeSequenceNode(GetNextId(), "NewNode");

				if (node)
				{
					BuildAllNodes();
					ed::SetNodePosition(node->nodeID, newNodePostion);
				}


				ImGui::EndPopup();
			}

			ImGui::PopStyleVar();
			ed::Resume();

			ed::End();
			
		}
		ImGui::EndChild();
	}

	ed::SetCurrentEditor(nullptr);
	//ImGui::ShowDemoWindow();

}

void AnimationTree::ShowLeftPane()
{
	auto& io = ImGui::GetIO();

	static bool isOpen;
	if (ImGui::Begin("Nodes", &isOpen, myNodePanelFlags))
	{

		if (ImGui::BeginMenuBar())
		{
			static bool save = false;
			static bool load = false;
			if (ImGui::BeginMenu("Menu"))
			{
				ImGui::MenuItem("Save", NULL, &save);
				/*ImGui::MenuItem("Save As", NULL);*/
				ImGui::MenuItem("Load", NULL, &load);
				ImGui::EndMenu();
			}

			if (save)
			{
				Save();
				save = false;
			}
			if (load)
			{
				Load();
				load = false;
			}


			ImGui::EndMenuBar();
		}


		if(ImGui::BeginChild("Nodes", ImVec2(0,0), true, 0))
		{
			if (ImGui::SmallButton("Create Node"))
			{
				SpawnTreeSequenceNode(GetNextId(), "NewNode");
			}

			ImGui::NewLine();

			for (auto& node : s_Nodes)
			{
				ImGui::Text(node.name.c_str());
				ImGui::NewLine();
			}
		}
		ImGui::EndChild();
	}

	ImGui::End();
}

Node* AnimationTree::SpawnTreeSequenceNode(int aID, std::string aNodeName)
{
	s_Nodes.emplace_back(aID, aNodeName.c_str());
	s_Nodes.back().Type = NodeType::Tree;
	s_Nodes.back().inputs.emplace_back(GetNextId(), "", PinType::Flow);
	s_Nodes.back().outputs.emplace_back(GetNextId(), "", PinType::Flow);

	BuildNode(&s_Nodes.back());

	return &s_Nodes.back();
}

void AnimationTree::BuildNode(Node* node)
{
	for (auto& input : node->inputs)
	{
		input.node = node;
		input.mode = PinMode::Input;
	}

	for (auto& output : node->outputs)
	{
		output.node = node;
		output.mode = PinMode::Output;
	}
}

void AnimationTree::Save()
{
	myDefaultConfig.SaveNodeSettings = [](ed::NodeId nodeId, const char* data, size_t size, ed::SaveReasonFlags reason, void* userPointer) -> bool
	{
		auto node = FindNode(nodeId);
		if (!node)
			return false;

		node->State.assign(data, size);

		return true;
	};

	YAML::Emitter out;
	//Save Nodes
	out << YAML::BeginMap;
	out << YAML::Key << "Nodes" << YAML::Value;
	{
		out << YAML::BeginMap;
		int nodeNumber = 0;
		for (auto& aNode : s_Nodes)
		{
			out << YAML::Key << nodeNumber << YAML::Value;
			{
				out << YAML::BeginMap;
				VT_SERIALIZE_PROPERTY_STRING("Name", aNode.name, out);
				VT_SERIALIZE_PROPERTY(ID, aNode.id, out);

				VT_SERIALIZE_PROPERTY(InputPin, aNode.inputs[0].id, out);
				VT_SERIALIZE_PROPERTY(InputPinType, (int)aNode.inputs[0].type, out);

				VT_SERIALIZE_PROPERTY(OutputPin, aNode.outputs[0].id, out);
				VT_SERIALIZE_PROPERTY(OutputPinType, (int)aNode.inputs[0].type, out);

				out << YAML::EndMap;
			}
			nodeNumber++;
		}
		out << YAML::EndMap;
	}
	//out << YAML::EndMap;

	//out << YAML::BeginMap;
	out << YAML::Key << "Links" << YAML::Value;
	{
		out << YAML::BeginMap;
		int linkNumber = 0;
		for (auto& aLink : s_Links)
		{
			out << YAML::Key << linkNumber << YAML::Value;
			{
				out << YAML::BeginMap;
				VT_SERIALIZE_PROPERTY(linkID, aLink.id, out);
				VT_SERIALIZE_PROPERTY(startPinID, aLink.startPinID, out);
				VT_SERIALIZE_PROPERTY(endPinID, aLink.endPinID, out);
				out << YAML::EndMap;
			}
			linkNumber++;
		}
		out << YAML::EndMap;
	}
	out << YAML::EndMap;

	std::wstring outputFolder = L"Editor/AnimationTree";

	if (CreateDirectory(outputFolder.c_str(), NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError())
	{
		std::ofstream fout("Editor/AnimationTree/Test.yaml");
		fout << out.c_str();
		fout.close();
	}
}

void AnimationTree::SaveAs()
{

}

void AnimationTree::Load()
{
	//Reset AnimationTree
	s_NextId = 1;
	s_Nodes.clear();
	s_Links.clear();

	//Load YAML File
	std::ifstream file("Editor/AnimationTree/Test.yaml");
	std::stringstream sstream;
	sstream << file.rdbuf();

	YAML::Node root = YAML::Load(sstream.str());
	YAML::Node Nodes = root["Nodes"];

	std::string aName = "";

	if (Nodes)
	{
		for (int currentNode = 0; currentNode < Nodes.size(); currentNode++) 
		{
			aName = Nodes[currentNode]["Name"].as<std::string>();
			//id = Nodes[currentNode]["ID"].as<int>();
			//inputPinID = Nodes[currentNode]["InputPin"].as<int>();
			//inputPinType = Nodes[currentNode]["InputPinType"].as<int>();
			//outputPinID = Nodes[currentNode]["OutputPin"].as<int>();
			//outputPinType = Nodes[currentNode]["OutputPinType"].as<int>();

			Node* node;
			node = SpawnTreeSequenceNode(GetNextId(), aName);
		}
	}

	BuildAllNodes();
	int startPinID = -1, endPinType = -1;
	YAML::Node Links = root["Links"];
	if (Links) 
	{
		int a = (int32_t)Links.size();

		for (int currentLink = 0; currentLink < Links.size(); currentLink++)
		{
			startPinID = Links[currentLink]["startPinID"].as<int>();
			endPinType = Links[currentLink]["endPinID"].as<int>();
			s_Links.push_back(Link(GetNextId(), startPinID, endPinType));
			s_Links.back().color = ImColor(0, 0, 255);
		}
	}

	file.close();

	//Set Up Config file
	ed::Config aConfig("Editor/AnimationTree/NodeEditor.json");

	myDefaultConfig.LoadNodeSettings = [](ed::NodeId nodeId, char* data, void* userPointer) -> size_t
	{
		auto node = FindNode(nodeId);
		if (!node)
			return 0;

		if (data != nullptr)
			memcpy(data, node->State.data(), node->State.size());
		return node->State.size();
	};

	

}

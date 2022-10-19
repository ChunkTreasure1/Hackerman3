#pragma once
#include "Sandbox/Window/EditorWindow.h"
#include <imgui.h>
#include <imgui_node_editor.h>
#include "../NodeEditor/Node.h"

namespace ed = ax::NodeEditor;

static ed::EditorContext* Context = nullptr;

static std::vector<ed::EditorContext*> s_Contexts;
static std::vector<Node>    s_Nodes;
static std::vector<Link>    s_Links;

static int currentContext = 0;
static int s_NextId = 1;
static int GetNextId()
{
    return s_NextId++;
}

static const float          s_TouchTime = 1.0f;
static std::map<ed::NodeId, float, NodeIdLess> s_NodeTouchTime;

static ed::LinkId GetNextLinkId()
{
    return ed::LinkId(GetNextId());
}

static void TouchNode(ed::NodeId id)
{
    s_NodeTouchTime[id] = s_TouchTime;
}

static float GetTouchProgress(ed::NodeId id)
{
    auto it = s_NodeTouchTime.find(id);
    if (it != s_NodeTouchTime.end() && it->second > 0.0f)
        return (s_TouchTime - it->second) / s_TouchTime;
    else
        return 0.0f;
}

static void UpdateTouch()
{
    const auto deltaTime = ImGui::GetIO().DeltaTime;
    for (auto& entry : s_NodeTouchTime)
    {
        if (entry.second > 0.0f)
            entry.second -= deltaTime;
    }
}

static Node* FindNode(ed::NodeId id)
{
    for (auto& node : s_Nodes)
        if (node.nodeID == id)
            return &node;

    return nullptr;
}

static Link* FindLink(ed::LinkId id)
{
    for (auto& link : s_Links)
        if (link.linkID == id)
            return &link;

    return nullptr;
}

static Pin* FindPin(ed::PinId id)
{
    if (!id)
        return nullptr;

    for (auto& node : s_Nodes)
    {
        for (auto& pin : node.inputs)
            if (pin.pinID == id)
                return &pin;

        for (auto& pin : node.outputs)
            if (pin.pinID == id)
                return &pin;
    }

    return nullptr;
}

static bool IsPinLinked(ed::PinId id)
{
    if (!id)
        return false;

    for (auto& link : s_Links)
        if (link.startPin == id || link.endPin == id)
            return true;

    return false;
}

//static bool CanCreateLink(Pin* a, Pin* b)
//{
//    if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
//        return false;
//
//    return true;
//}

class AnimationTree : public EditorWindow
{
public:
	AnimationTree();
	void UpdateMainContent() override;

private:

	Node* SpawnTreeSequenceNode(int aID, std::string aNodeName);

	void ShowLeftPane();
	void BuildNode(Node* node);
	void BuildAllNodes()
	{
		for (auto& node : s_Nodes)
		{
			BuildNode(&node);
		}
	}	
    
    ed::Config myDefaultConfig;

    //ed::Config myDefaultConfig = ed::Config("Editor/AnimationTree/Default.json");
    //ed::Config myOtherDefaultConfig = ed::Config("Editor/AnimationTree/DefaultOther.json");

	void Save();
	void SaveAs();
	void Load();

};




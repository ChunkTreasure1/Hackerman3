#pragma once

#include <imgui_node_editor.h>

#include <string>
#include <vector>

namespace ed = ax::NodeEditor;

enum class PinType
{
	Flow,
	Bool,
	Int,
	Float,
	String,
	Object,
	Function,
	Delegate
};

enum class PinMode
{
	Output,
	Input
};

enum class NodeType
{
	Blueprint,
	Simple,
	Tree,
	Comment,
	Houdini
};

struct Node;

struct Pin
{
	ed::PinId pinID;
	int id;
	::Node* node;
	std::string name;
	PinType type;
	PinMode mode;

	Pin(int aID, const char* aName, PinType aType)
		: pinID(aID), id(aID), node(nullptr), name(aName), type(aType), mode(PinMode::Input)
	{

	}
};

struct Node
{
	ed::NodeId nodeID;
	int id;
	std::string name;

	std::vector<Pin> inputs;
	std::vector<Pin> outputs;
	ImColor color;
	NodeType Type;
	ImVec2 size;

	std::string State;
	std::string SavedState;

	Node(int aID, const char* aName, ImColor aColor = ImColor(255, 255, 255)) :
		nodeID(aID), id(aID), name(aName), color(aColor), Type(NodeType::Tree), size(512, 512)
	{
	}
};

struct Link
{
	ed::LinkId linkID = 0;
	int id = 0;

	ed::PinId startPin = 0;
	int startPinID = 0;
	ed::PinId endPin = 0;
	int endPinID = 0;


	ImColor color;

	//Link(ed::LinkId aId, ed::PinId aStartId, ed::PinId aEndId)
	//	: linkID(aId), startPin(aStartId), endPin(aEndId)
	//{
	//}

	Link(int aId, int aStartId, int aEndId)
		: linkID(aId), id(aId), startPin(aStartId), startPinID(aStartId), endPin(aEndId), endPinID(aEndId)
	{
	}

	Link(ed::LinkId aId, int aStartId, int aEndId)
		: linkID(aId), startPin(aStartId), startPinID(aStartId), endPin(aEndId), endPinID(aEndId)
	{
	}
};

struct NodeIdLess
{
	bool operator()(const ed::NodeId& lhs, const ed::NodeId& rhs) const
	{
		return lhs.AsPointer() < rhs.AsPointer();
	}
};




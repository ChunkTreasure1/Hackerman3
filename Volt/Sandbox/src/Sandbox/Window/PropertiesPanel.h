#pragma once

#include "Sandbox/Window/EditorWindow.h"

#include <Volt/Components/Components.h>
#include <Volt/Scene/Entity.h>
#include <Volt/Scene/Scene.h>
#include <vector>
#include <variant>
#include "Sandbox/EditorCommand.h"

enum class PropertyEventType
{
	Position, 
	Rotation,
	Scale
};

struct PropertyEvent
{
	Wire::EntityId myEntityId;
	PropertyEventType myType;
	std::variant<gem::vec3> myValue;
};

class PropertiesPanel : public EditorWindow
{
public:
	PropertiesPanel(Ref<Volt::Scene>& currentScene);
	void UpdateMainContent() override;

private:
	void AddComponentPopup();

	Ref<Volt::Scene>& myCurrentScene;

	bool myMidEvent = false;
	std::shared_ptr<PropertyEvent> myLastValue;
	gem::vec3 myLastValue2;
	std::vector<std::shared_ptr<PropertyEvent>> myUndoList;
	std::vector<std::shared_ptr<PropertyEvent>> myRedoList;
	int myMaxEventListSize;
};
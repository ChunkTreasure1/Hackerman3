#pragma once
#include <Volt/Scene/Scene.h>
#include "Volt/Core/Base.h"
#include "Sandbox/Window/EditorWindow.h"
#include <vector>

class AbilityStatsPanel : public EditorWindow
{
public:
	AbilityStatsPanel();
	void UpdateMainContent() override;
	void InitNames();

private:

	std::vector<std::string> myAbilityNames;
};
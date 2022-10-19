#pragma once

#include "Volt/Core/Base.h"

#include <imgui.h>

struct GLFWwindow;

namespace Volt
{
	class ImGuiImplementation
	{
	public:
		ImGuiImplementation();
		~ImGuiImplementation();

		void Begin();
		void End();

		static Scope<ImGuiImplementation> Create();

	private:
		ImFont* myFont;
		GLFWwindow* myWindowPtr;
	};
}
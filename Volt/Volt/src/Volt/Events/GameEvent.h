#pragma once

#include "Event.h"

#include "Volt/Asset/Asset.h"

#include <sstream>
#include <vector>
#include <filesystem>

namespace Volt
{
	class OnRespawnEvent : public Event
	{
	public:
		OnRespawnEvent() = default;

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "OnRespawn" << std::endl;
			return ss.str();
		}

		EVENT_CLASS_TYPE(OnRespawn);
		EVENT_CLASS_CATEGORY(EventCategoryGame);
	};

}
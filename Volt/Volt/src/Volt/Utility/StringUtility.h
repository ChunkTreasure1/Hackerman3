#pragma once

#include <string>
#include <algorithm>
#include <filesystem>

namespace Utility
{
	inline std::string ToLower(const std::string& str)
	{
		std::string newStr(str);
		std::transform(str.begin(), str.end(), newStr.begin(), ::tolower);

		return newStr;
	}

	inline std::string RemoveTrailingZeroes(const std::string& string)
	{
		std::string newStr = string;
		newStr.erase(newStr.find_last_not_of('0') + 1, std::string::npos);

		if (!newStr.empty() && newStr.back() == '.')
		{
			newStr.pop_back();
		}

		return newStr;
	}
}
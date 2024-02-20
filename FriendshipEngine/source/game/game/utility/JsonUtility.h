#pragma once

#include <nlohmann/json_fwd.hpp>

class JsonUtility
{
public:
	static nlohmann::json OpenJson(const std::string& aFileName);
	static void OverwriteJson(const std::string& aFileName, nlohmann::json aJson);
};
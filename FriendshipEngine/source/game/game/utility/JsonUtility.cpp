#include "pch.h"
#include "JsonUtility.h"
#include <engine/utility/Error.h>
#include <engine/Paths.h>
#include <ostream>
#include <nlohmann/json.hpp>

nlohmann::json JsonUtility::OpenJson(const std::string& aFileName)
{
	std::ifstream dataFile(aFileName);
	if (dataFile.fail())
	{
		PrintE("Could not open \"" + aFileName + "\" to read data");
		return nullptr;
	}

	nlohmann::json json = nlohmann::json::parse(dataFile);
	dataFile.close();

	return json;
}

void JsonUtility::OverwriteJson(const std::string& aFileName, nlohmann::json aJson)
{
	std::ofstream dataFile(aFileName);
	if (dataFile.fail())
	{
		PrintE("Could not open \"" + aFileName + "\" to save data");
		return;
	}

	dataFile << aJson;

	dataFile.close();
}
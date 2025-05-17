#include "pch.h"

#include <fstream>
#include <filesystem>

#include "ScriptManager.h"
#include "scene/SceneCommon.h"

#include <nlohmann/json.hpp>
#include "Script.h"
#include "../utility/JsonUtility.h"
#include "utility/ScriptJson.h"
#include "ScriptStringRegistry.h"
#include "ScriptRuntimeInstance.h"

#include <engine/Paths.h>
#include <engine/utility/Error.h>


std::unordered_map<std::string_view, ScriptManager::InternalScriptData> ScriptManager::myLoadedScripts;

std::shared_ptr<const Script> ScriptManager::GetScript(std::string_view aLevel, std::string_view aName)
{
	if (!GetEditableScript(aLevel, aName))
		return nullptr;

	ScriptManager::InternalScriptData& data = myLoadedScripts[aName];

	int sequenceNumber = data.script->GetSequenceNumber();
	if (data.latestSnapshotSequenceNumber == sequenceNumber)
		return data.latestSnapshot;

	std::shared_ptr<Script> newSnapshot = std::make_shared<Script>();
	ScriptJson json;
	data.script->WriteToJson(json);
	newSnapshot->LoadFromJson(json);
	newSnapshot->SetSequenceNumber(data.script->GetSequenceNumber());

	data.latestSnapshotSequenceNumber = sequenceNumber;
	data.latestSnapshot = newSnapshot;

	return newSnapshot;
}

std::vector<std::unique_ptr<ScriptRuntimeInstance>> ScriptManager::GetLevelScripts(const std::string& aLevelName)
{
	std::vector<std::unique_ptr<ScriptRuntimeInstance>> scripts;

	std::string DIRECTORY = aLevelName + "/scripts/";

	if (!std::filesystem::exists(RELATIVE_IMPORT_PATH + DIRECTORY))
		return scripts;

	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(RELATIVE_IMPORT_PATH + DIRECTORY))
	{
		std::shared_ptr<const Script> script = ScriptManager::GetScript(aLevelName, entry.path().filename().string());
		scripts.push_back(std::make_unique<ScriptRuntimeInstance>(script));
	}

	return scripts;
}

//void ScriptManager::SaveLevelScript(const std::string& aActiveLevel, const std::string_view& aActiveScript)
//{
//	//std::string path = RELATIVE_IMPORT_DATA_PATH + (std::string)"scripts/Scripts.json";
//	//nlohmann::json scriptFile = JsonUtility::OpenJson(path);
//	//nlohmann::json jsonLevels = scriptFile["levels"];
//
//	std::string activeLevel(aActiveLevel);
//	std::string activeScript(aActiveScript);
//
//	//size_t found = activeScript.find(".json");
//	//if (found == std::string::npos)
//	//	activeScript += ".json";
//
//	std::string DIRECTORY = activeLevel + "/scripts/";
//
//	//bool levelExists = false;
//	//int levelIndex = -1;
//
//	//for (size_t i = 0; i < jsonLevels.size(); i++)
//	//{
//	//	std::string fileLevelName = jsonLevels[i]["name"];
//
//	//	if (fileLevelName == activeLevel)
//	//	{
//	//		levelIndex = (int)i;
//	//		levelExists = true;
//	//	}
//	//}
//
//	//Add file extension if missing
//
//	//if (levelExists)
//	//{
//	//	nlohmann::json& levelScripts = jsonLevels[levelIndex]["scripts"];
//	//	
//	//	for (size_t i = 0; i < levelScripts.size(); i++)
//	//	{
//	//		if (levelScripts[i] == activeScript)
//	//			return;
//	//	}
//
//	//	levelScripts.push_back(activeScript);
//	//}
//	//else
//	//{
//	//	nlohmann::json newLevel;
//	//	newLevel["name"] = aActiveLevel;
//	//	newLevel["scripts"].push_back(activeScript);
//
//	//	jsonLevels.push_back(newLevel);
//	//}
//
//	//scriptFile["levels"] = jsonLevels;
//
//	//std::ofstream out(path);
//	//out << scriptFile;
//	//out.close();
//}

Script* ScriptManager::GetEditableScript(std::string_view aLevel, std::string_view aName)
{
	auto it = myLoadedScripts.find(aName);
	if (it != myLoadedScripts.end())
		return it->second.script.get();

	std::string levelFolder = std::string(aLevel) + "/scripts/";

	nlohmann::json scriptFile = JsonUtility::OpenJson(RELATIVE_IMPORT_PATH + levelFolder + std::string(aName));
	ScriptJson data = { scriptFile };

	ScriptStringId nameStringId = ScriptStringRegistry::RegisterOrGetStringId(aName);
	std::string_view nameStringView = ScriptStringRegistry::GetStringFromStringId(nameStringId);

	myLoadedScripts.insert({ nameStringView, InternalScriptData{std::make_unique<Script>(), nullptr, -1} });
	Script& script = *myLoadedScripts[nameStringView].script;

	script.LoadFromJson(data);

	return &script;
}

void ScriptManager::AddEditableScript(std::string_view name, std::unique_ptr<Script>&& script)
{
	ScriptStringId nameStringId = ScriptStringRegistry::RegisterOrGetStringId(name);
	std::string_view nameStringView = ScriptStringRegistry::GetStringFromStringId(nameStringId);

	myLoadedScripts.insert({ nameStringView, InternalScriptData{std::move(script), nullptr, -1} });
}
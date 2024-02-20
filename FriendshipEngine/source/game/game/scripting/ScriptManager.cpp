#include "pch.h"

#include "ScriptManager.h"

#include "Script.h"
#include "utility/ScriptJson.h"
#include "ScriptStringRegistry.h"
#include <engine/Paths.h>
#include <engine/utility/Error.h>
#include <fstream>

std::unordered_map<std::string_view, ScriptManager::InternalScriptData> ScriptManager::myLoadedScripts;

std::shared_ptr<const Script> ScriptManager::GetScript(std::string_view aName)
{
	if (!GetEditableScript(aName))
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

Script* ScriptManager::GetEditableScript(std::string_view aName)
{
	auto it = myLoadedScripts.find(aName);
	if (it != myLoadedScripts.end())
		return it->second.script.get();

	std::string path = RELATIVE_EDITOR_DATA_PATH + (std::string)"scripts/" + std::string(aName) + ".json";

	std::ifstream file(path);
	if (!file.is_open())
	{
		PrintW("[ScriptManager.cpp] Could not open Editable Script!");
		return nullptr;
	}


	ScriptJson data = { nlohmann::json::parse(file) };
	file.close();

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
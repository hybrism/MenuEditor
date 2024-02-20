#include "pch.h"

#include "ScriptStringRegistry.h"

std::vector<std::unique_ptr<std::string>> ScriptStringRegistry::myStrings;
std::unordered_map<std::string_view, ScriptStringId> ScriptStringRegistry::myStringToScriptStringId;

ScriptStringId ScriptStringRegistry::RegisterOrGetStringId(std::string_view string)
{
	auto it = myStringToScriptStringId.find(string);
	if (it != myStringToScriptStringId.end())
		return it->second;

	ScriptStringId id = { (unsigned int)myStrings.size() };

	myStrings.emplace_back(std::make_unique<std::string>(string));
	myStringToScriptStringId[*myStrings.back()] = id;

	return id;
}

std::string_view ScriptStringRegistry::GetStringFromStringId(ScriptStringId stringId)
{
	assert("Invalid string id" && stringId.id < myStrings.size());
	return *myStrings[stringId.id];
}

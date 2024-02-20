#pragma once

#include "ScriptCommon.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

class ScriptStringRegistry
{
private:
	static std::vector<std::unique_ptr<std::string>> myStrings;
	static std::unordered_map<std::string_view, ScriptStringId> myStringToScriptStringId;

public:
	static ScriptStringId RegisterOrGetStringId(std::string_view string);
	static std::string_view GetStringFromStringId(ScriptStringId stringId);
};

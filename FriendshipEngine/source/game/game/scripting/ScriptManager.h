#pragma once

#include <unordered_map>
#include <memory>
#include <string_view>

class Script;
class ScriptRuntimeInstance;

class ScriptManager
{
	struct InternalScriptData
	{
		std::unique_ptr<Script> script;
		std::shared_ptr<const Script> latestSnapshot;
		int latestSnapshotSequenceNumber = -1;
	};

public:
	/// <summary>
	/// Gets the latest version of a script. If a script is edited, this script will not be updated. This way a script can be executed while being edited without risking inconsistent data.
	/// To get the latest version, this function has to be called again.
	/// This means that multiple versions of a script can exist, depending on when this got called.
	/// </summary>
	static std::shared_ptr<const Script> GetScript(std::string_view aLevel, std::string_view aName);

	static std::vector<std::unique_ptr<ScriptRuntimeInstance>> GetLevelScripts(const std::string& aLevelName);

	//static void SaveLevelScript(const std::string& aActiveLevel, const std::string_view& aActiveScript);

	/// <summary>
	/// Gets an editable version of a script. Only a single editable version exists 
	/// </summary>
	static Script* GetEditableScript(std::string_view aLevel, std::string_view aName);

	static void AddEditableScript(std::string_view aName, std::unique_ptr<Script>&& aScript);

private:
	static std::unordered_map<std::string_view, InternalScriptData> myLoadedScripts;
	
	const char* SCRIPT_DIRECTORY = "scripts/";
};


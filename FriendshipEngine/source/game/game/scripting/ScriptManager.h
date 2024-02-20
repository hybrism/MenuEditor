#pragma once

#include <unordered_map>
#include <memory>
#include <string_view>

class Script;
class ScriptManager
{
	struct InternalScriptData
	{
		std::unique_ptr<Script> script;
		std::shared_ptr<const Script> latestSnapshot;
		int latestSnapshotSequenceNumber = -1;
	};

	static std::unordered_map<std::string_view, InternalScriptData> myLoadedScripts;

public:
	/// <summary>
	/// Gets the latest version of a script. If a script is edited, this script will not be updated. This way a script can be executed while being edited without risking inconsistent data.
	/// To get the latest version, this function has to be called again.
	/// This means that multiple versions of a script can exist, depending on when this got called.
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	static std::shared_ptr<const Script> GetScript(std::string_view aName);

	/// <summary>
	/// Gets an editable version of a script. Only a single editable version exists 
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	static Script* GetEditableScript(std::string_view aName);

	static void AddEditableScript(std::string_view aName, std::unique_ptr<Script>&& aScript);
};


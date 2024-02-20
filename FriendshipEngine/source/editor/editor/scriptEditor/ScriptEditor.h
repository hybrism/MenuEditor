#pragma once
#include <map>

#include "ScriptEditorSelection.h"
#include "ScriptEditorNodeColors.h"
#include <game/scripting/Script.h>

struct ImNodesEditorContext;

class Script;
class MoveNodesCommand;

class ScriptEditor
{
	struct EditorScriptData
	{
		Script* script;
		ScriptEditorSelection selection = {};
		ImNodesEditorContext* nodeEditorContext = nullptr;
		ScriptPinId inProgressLinkPin = { ScriptPinId::InvalidId };
		ScriptNodeId hoveredNode = { ScriptNodeId::InvalidId };
		int latestSavedSequenceNumber = 0;
	};

	std::map<std::string_view, EditorScriptData> myOpenScripts;
	std::string_view myActiveScript;

	std::shared_ptr<MoveNodesCommand> myInProgressMove;
public:
	ScriptEditor();
	~ScriptEditor();

	void Init();
	void Update();

private:
	std::string SCRIPT_FILEPATH;
};
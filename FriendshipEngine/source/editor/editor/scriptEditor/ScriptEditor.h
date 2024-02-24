#pragma once
#include <map>

#include "ScriptEditorSelection.h"
#include "ScriptEditorNodeColors.h"
#include <game/scripting/Script.h>
#include <shared/postMaster/Observer.h>

struct ImNodesEditorContext;
struct EditorUpdateContext;

class Script;
class MoveNodesCommand;
class ScriptEditor : public FE::Observer
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

public:
	ScriptEditor();
	~ScriptEditor();

	void Init();
	void Update(const EditorUpdateContext& aContext);

private:
	std::map<std::string_view, EditorScriptData> myScripts;

	std::map<std::string_view, std::vector<std::string_view>> myLevels;

	std::map<std::string_view, EditorScriptData> myOpenScripts;

	std::string_view myActiveScript;
	std::string myActiveLevel;

	std::shared_ptr<MoveNodesCommand> myInProgressMove;

	std::string SCRIPT_FILEPATH;
	std::string LEVEL_FILEPATH;

	//IMGUI, separated into functions for easier navigation & handling
	void SelectScript();
	void SaveButton(EditorScriptData& aActiveScript);
	void RevertButton(EditorScriptData& aActiveScript);
	void UndoButton();
	void RedoButton();

	void NodeEditor(EditorScriptData& aActiveScript, const EditorUpdateContext& aContext);

	// Inherited via Observer
	void RecieveMessage(const FE:: Message& aMessage) override;
	void OnNewLevelLoaded(const std::string& aLevelName);
};
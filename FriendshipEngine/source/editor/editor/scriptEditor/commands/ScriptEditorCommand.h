#pragma once
#include <memory>
#include <shared/command/AbstractCommand.h>
#include <game/scripting/ScriptCommon.h>

class Script;
struct ScriptEditorSelection;
class ScriptNodeBase;

struct CommandNodeData
{
	ScriptNodeTypeId typeId;
	ScriptStringId instanceName;
	Vector2f pos;
	std::unique_ptr<ScriptNodeBase> node;
};

class ScriptEditorCommand : public AbstractCommand
{
protected:
	Script& myScript;
	ScriptEditorSelection& mySelection;
	int mySequenceNumber = -1;
public:
	ScriptEditorCommand(Script& script, ScriptEditorSelection& selection)
		: myScript(script)
		, mySelection(selection)
	{}

	void Execute() override final;
	void Undo() override final;

	virtual void ExecuteImpl() = 0;
	virtual void UndoImpl() = 0;
};
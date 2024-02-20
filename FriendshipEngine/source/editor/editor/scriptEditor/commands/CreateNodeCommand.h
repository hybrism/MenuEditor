#pragma once
#include "ScriptEditorCommand.h"
#include <unordered_map>

class CreateNodeCommand : public ScriptEditorCommand
{
	ScriptNodeId myNodeId;
	CommandNodeData myNodeData;
	std::unordered_map<ScriptPinId, ScriptPin> myPins;

public:
	CreateNodeCommand(Script& script, ScriptEditorSelection& selection, ScriptNodeTypeId typeId, Vector2f pos)
		: ScriptEditorCommand(script, selection)
		, myNodeId{ ScriptNodeId::InvalidId }
	{
		myNodeData.pos = pos;
		myNodeData.typeId = typeId;
	}

	void ExecuteImpl() override;
	void UndoImpl() override;
};
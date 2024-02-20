#pragma once
#include "ScriptEditorCommand.h"
#include <unordered_map>

class MoveNodesCommand : public ScriptEditorCommand
{
	ScriptNodeId myNode;
	std::unordered_map<ScriptNodeId, Vector2f> myFromPositions;
	std::unordered_map<ScriptNodeId, Vector2f> myToPositions;
public:
	MoveNodesCommand(Script& script, ScriptEditorSelection& selection)
		: ScriptEditorCommand(script, selection)
	{}

	void SetPosition(ScriptNodeId id, Vector2f oldPosition, Vector2f newPosition)
	{
		if (myFromPositions.find(id) == myFromPositions.end())
			myFromPositions[id] = oldPosition;

		myToPositions[id] = newPosition;
	}

	void ExecuteImpl() override;
	void UndoImpl() override;
};
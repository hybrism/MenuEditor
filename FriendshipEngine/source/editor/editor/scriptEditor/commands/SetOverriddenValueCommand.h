#pragma once
#include "ScriptEditorCommand.h"

class SetOverriddenValueCommand : public ScriptEditorCommand
{
	ScriptPinId myPinId;
	ScriptLinkData myNewData;
	ScriptLinkData myOldData;

public:
	SetOverriddenValueCommand(Script& script, ScriptEditorSelection& selection, ScriptPinId pinId, ScriptLinkData data)
		: ScriptEditorCommand(script, selection)
		, myPinId(pinId)
		, myNewData(data)
	{}

	void ExecuteImpl() override;
	void UndoImpl() override;
};
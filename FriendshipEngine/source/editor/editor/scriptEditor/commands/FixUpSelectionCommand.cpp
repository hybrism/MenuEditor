#include "FixUpSelectionCommand.h"
#include "../ScriptEditorSelection.h"

void FixUpSelectionCommand::ExecuteImpl()
{
	mySelectedLinks = mySelection.mySelectedLinks;
	mySelectedNodes = mySelection.mySelectedNodes;
	myCommand->Execute();
}

void FixUpSelectionCommand::UndoImpl()
{
	myCommand->Undo();
	mySelection.mySelectedLinks = mySelectedLinks;
	mySelection.mySelectedNodes = mySelectedNodes;
}


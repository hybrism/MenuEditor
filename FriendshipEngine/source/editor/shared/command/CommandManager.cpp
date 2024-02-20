#include "CommandManager.h"

#include "AbstractCommand.h"

#include <stack>

static std::stack<std::shared_ptr<AbstractCommand>> locUndoStack;
static std::stack<std::shared_ptr<AbstractCommand>> locRedoStack;

void CommandManager::DoCommand(const std::shared_ptr<AbstractCommand>& aCommand)
{
	aCommand->Execute();

	locUndoStack.push(aCommand);

	//Empty redoStack
	while (!locRedoStack.empty())
	{
		locRedoStack.pop();
	}
}

void CommandManager::Clear()
{
	while (!locUndoStack.empty())
	{
		locUndoStack.pop();
	}

	while (!locRedoStack.empty())
	{
		locRedoStack.pop();
	}
}

void CommandManager::Undo()
{
	if (CanUndo())
	{
		locUndoStack.top()->Undo();
		locRedoStack.push(locUndoStack.top());
		locUndoStack.pop();
	}
}

void CommandManager::Redo()
{
	if (CanRedo())
	{
		locRedoStack.top()->Execute();
		locUndoStack.push(locRedoStack.top());
		locRedoStack.pop();
	}
}

bool CommandManager::CanUndo()
{
	return !locUndoStack.empty();
}

bool CommandManager::CanRedo()
{
	return !locRedoStack.empty();
}

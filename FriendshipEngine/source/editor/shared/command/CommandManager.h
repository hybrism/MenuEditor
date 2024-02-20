#pragma once

#include <memory>

class AbstractCommand;

class CommandManager
{
public:
	static void DoCommand(const std::shared_ptr<AbstractCommand>& aCommand);
	static void Clear();

	static void Undo();
	static void Redo();

	static bool CanUndo();
	static bool CanRedo();
};
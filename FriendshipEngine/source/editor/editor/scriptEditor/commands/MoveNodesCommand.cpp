#include "MoveNodesCommand.h"
#include <game/scripting/Script.h>

void MoveNodesCommand::ExecuteImpl()
{
	for (std::pair<const ScriptNodeId, Vector2f> p : myToPositions)
	{
		myScript.SetPosition(p.first, p.second);
	}
}
void MoveNodesCommand::UndoImpl()
{
	for (std::pair<const ScriptNodeId, Vector2f> p : myFromPositions)
	{
		myScript.SetPosition(p.first, p.second);
	}
}
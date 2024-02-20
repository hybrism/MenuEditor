#pragma once
#include <vector>
#include <game/scripting/ScriptCommon.h>

struct ScriptEditorSelection
{
	std::vector<ScriptNodeId> mySelectedNodes;
	std::vector<ScriptLinkId> mySelectedLinks;
};
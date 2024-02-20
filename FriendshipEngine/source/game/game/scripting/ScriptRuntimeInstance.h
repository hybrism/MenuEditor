#pragma once

#include "Script.h"
#include "nodes/ScriptNodeBase.h"
#include <vector>

struct ScriptUpdateContext;

class ScriptRuntimeInstance
{
	std::shared_ptr<const Script> myScript;

	std::vector<std::unique_ptr<ScriptNodeRuntimeInstanceBase>> myNodeInstances;
	std::vector<ScriptNodeId> myActiveNodes;

public:
	ScriptRuntimeInstance(std::shared_ptr<const Script>& script);

	void Init();
	void Update(const ScriptUpdateContext& context);
	void TriggerPin(ScriptPinId pin, const ScriptUpdateContext& context);

	const Script& GetScript() const;
	ScriptNodeRuntimeInstanceBase* GetRuntimeInstance(ScriptNodeId nodeId);
	void ActivateNode(ScriptNodeId nodeId);
	void DeactivateNode(ScriptNodeId nodeId);
};
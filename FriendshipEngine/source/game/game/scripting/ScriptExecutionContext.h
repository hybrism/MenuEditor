#pragma once
#include "ScriptCommon.h"

class ScriptExecutionContext
{
public:
	ScriptExecutionContext(ScriptRuntimeInstance& scriptRuntimeInstance, const ScriptUpdateContext& updateContext, ScriptNodeId nodeId, ScriptNodeRuntimeInstanceBase* nodeRuntimeInstance);
	~ScriptExecutionContext();

	void TriggerOutputPin(ScriptPinId outputPin);
	ScriptLinkData ReadInputPin(ScriptPinId inputPin);

	const ScriptUpdateContext& GetUpdateContext() { return myUpdateContext; }
	ScriptNodeRuntimeInstanceBase* GetRuntimeInstanceData() { return myNodeRuntimeInstance; }

private:
	static constexpr int MAX_TRIGGERED_OUTPUTS = 8;

	ScriptRuntimeInstance& myScriptRuntimeInstance;
	const ScriptUpdateContext& myUpdateContext;
	ScriptNodeId myNodeId;
	ScriptNodeRuntimeInstanceBase* myNodeRuntimeInstance;

	ScriptPinId myTriggeredOutputQueue[MAX_TRIGGERED_OUTPUTS];
	int myTriggeredOutputCount;

};

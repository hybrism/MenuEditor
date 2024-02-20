#include "pch.h"

#include "ScriptNodeBase.h"
#include "ScriptNodeTypeRegistry.h"

#include "../ScriptExecutionContext.h"
#include "../ScriptCommon.h"
#include "../ScriptStringRegistry.h"

class StartNode : public ScriptNodeBase
{
	ScriptPinId myOutputPin;

public:
	void Init(const ScriptCreationContext& context) override
	{
		ScriptPin outputPin = {};
		outputPin.dataType = ScriptLinkDataType::Flow;
		outputPin.name = ScriptStringRegistry::RegisterOrGetStringId("Start");
		outputPin.node = context.GetNodeId();
		outputPin.role = ScriptPinRole::Output;

		myOutputPin = context.FindOrCreatePin(outputPin);
	}

	ScriptNodeResult Execute(ScriptExecutionContext& context, ScriptPinId) const override
	{
		context.TriggerOutputPin(myOutputPin);

		return ScriptNodeResult::Finished;
	}

	bool ShouldExecuteAtStart() const override { return true; }
};

class UpdateNode : public ScriptNodeBase
{
	ScriptPinId myOutputPin;

public:
	void Init(const ScriptCreationContext& context) override
	{
		ScriptPin outputPin = {};
		outputPin.dataType = ScriptLinkDataType::Flow;
		outputPin.name = ScriptStringRegistry::RegisterOrGetStringId("Update");
		outputPin.node = context.GetNodeId();
		outputPin.role = ScriptPinRole::Output;

		myOutputPin = context.FindOrCreatePin(outputPin);
	}

	ScriptNodeResult Execute(ScriptExecutionContext& context, ScriptPinId) const override
	{
		context.TriggerOutputPin(myOutputPin);

		return ScriptNodeResult::KeepRunning;
	}

	bool ShouldExecuteEachFrame() const override { return true; }
};

void RegisterCommonNodes()
{
	ScriptNodeTypeRegistry::RegisterType<StartNode>("Common/Start", "A node that executes once when the script starts");
	ScriptNodeTypeRegistry::RegisterType<UpdateNode>("Common/Update", "A node that executes once every frame");
}
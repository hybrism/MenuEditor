#include "pch.h"

#include "ScriptNodeBase.h"
#include "ScriptNodeTypeRegistry.h"

#include "../ScriptExecutionContext.h"
#include "../ScriptCommon.h"
#include "../ScriptStringRegistry.h"

class StartNode : public ScriptNodeBase
{
	ScriptPinId myPositionOutputPin;

public:
	void Init(const ScriptCreationContext& context) override
	{
		ScriptPin outputPin = {};
		outputPin.dataType = ScriptLinkDataType::Flow;
		outputPin.name = ScriptStringRegistry::RegisterOrGetStringId("Start");
		outputPin.node = context.GetNodeId();
		outputPin.role = ScriptPinRole::Output;

		myPositionOutputPin = context.FindOrCreatePin(outputPin);
	}

	ScriptNodeResult Execute(ScriptExecutionContext& context, ScriptPinId) const override
	{
		context.TriggerOutputPin(myPositionOutputPin);

		return ScriptNodeResult::Finished;
	}

	bool ShouldExecuteAtStart() const override { return true; }
};

class UpdateNode : public ScriptNodeBase
{
	ScriptPinId myPositionOutputPin;

public:
	void Init(const ScriptCreationContext& context) override
	{
		ScriptPin outputPin = {};
		outputPin.dataType = ScriptLinkDataType::Flow;
		outputPin.name = ScriptStringRegistry::RegisterOrGetStringId("Update");
		outputPin.node = context.GetNodeId();
		outputPin.role = ScriptPinRole::Output;

		myPositionOutputPin = context.FindOrCreatePin(outputPin);
	}

	ScriptNodeResult Execute(ScriptExecutionContext& context, ScriptPinId) const override
	{
		context.TriggerOutputPin(myPositionOutputPin);

		return ScriptNodeResult::KeepRunning;
	}

	bool ShouldExecuteEachFrame() const override { return true; }
};

class BranchNode : public ScriptNodeBase
{
	ScriptPinId myOutputTruePin;
	ScriptPinId myOutputFalsePin;
	ScriptPinId myFlowPin;
	ScriptPinId myConditionPin;

public:
	void Init(const ScriptCreationContext& aContext) override
	{
		{
			ScriptPin outputPin = {};
			outputPin.dataType = ScriptLinkDataType::Flow;
			outputPin.name = ScriptStringRegistry::RegisterOrGetStringId("Run");
			outputPin.node = aContext.GetNodeId();
			outputPin.role = ScriptPinRole::Input;

			myFlowPin = aContext.FindOrCreatePin(outputPin);
		}

		{
			ScriptPin outputPin = {};
			outputPin.dataType = ScriptLinkDataType::Bool;
			outputPin.name = ScriptStringRegistry::RegisterOrGetStringId("Condition");
			outputPin.node = aContext.GetNodeId();
			outputPin.role = ScriptPinRole::Input;
			outputPin.defaultValue.data = true;

			myConditionPin = aContext.FindOrCreatePin(outputPin);
		}

		{
			ScriptPin outputPin = {};
			outputPin.dataType = ScriptLinkDataType::Flow;
			outputPin.name = ScriptStringRegistry::RegisterOrGetStringId("True");
			outputPin.node = aContext.GetNodeId();
			outputPin.role = ScriptPinRole::Output;

			myOutputTruePin = aContext.FindOrCreatePin(outputPin);
		}

		{
			ScriptPin outputPin = {};
			outputPin.dataType = ScriptLinkDataType::Flow;
			outputPin.name = ScriptStringRegistry::RegisterOrGetStringId("False");
			outputPin.node = aContext.GetNodeId();
			outputPin.role = ScriptPinRole::Output;

			myOutputFalsePin = aContext.FindOrCreatePin(outputPin);
		}
	}

	ScriptNodeResult Execute(ScriptExecutionContext& context, ScriptPinId) const override
	{
		bool condition = std::get<bool>(context.ReadInputPin(myConditionPin).data);

		if (condition)
			context.TriggerOutputPin(myOutputTruePin);
		else
			context.TriggerOutputPin(myOutputFalsePin);

		return ScriptNodeResult::Finished;
	}

};

void RegisterCommonNodes()
{
	ScriptNodeTypeRegistry::RegisterType<StartNode>("Common/Start", "A node that executes once when the script starts");
	ScriptNodeTypeRegistry::RegisterType<UpdateNode>("Common/Update", "A node that executes once every frame");
	ScriptNodeTypeRegistry::RegisterType<BranchNode>("Flow/Branch", "A node that branches");
}
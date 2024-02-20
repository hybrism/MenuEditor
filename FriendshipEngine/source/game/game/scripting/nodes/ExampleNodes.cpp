#include "pch.h"

#include "ScriptNodeBase.h"
#include "ScriptNodeTypeRegistry.h"

#include "../ScriptExecutionContext.h"
#include "../ScriptCommon.h"
#include "../ScriptStringRegistry.h"

class PrintIntNode : public ScriptNodeBase
{
	ScriptPinId myIntPinId;
	ScriptPinId myOutPinId;

public:
	void Init(const ScriptCreationContext& context) override
	{
		{
			ScriptPin flowPin = {};
			flowPin.dataType = ScriptLinkDataType::Flow;
			flowPin.name = ScriptStringRegistry::RegisterOrGetStringId("Run");
			flowPin.node = context.GetNodeId();
			flowPin.role = ScriptPinRole::Input;

			context.FindOrCreatePin(flowPin);
		}

		{
			ScriptPin flowOutPin = {};
			flowOutPin.dataType = ScriptLinkDataType::Flow;
			flowOutPin.name = ScriptStringRegistry::RegisterOrGetStringId("");
			flowOutPin.node = context.GetNodeId();
			flowOutPin.role = ScriptPinRole::Output;

			myOutPinId = context.FindOrCreatePin(flowOutPin);
		}

		{
			ScriptPin intPin = {};
			intPin.dataType = ScriptLinkDataType::Int;
			intPin.name = ScriptStringRegistry::RegisterOrGetStringId("Value");
			intPin.node = context.GetNodeId();
			intPin.defaultValue = { 0 };
			intPin.role = ScriptPinRole::Input;

			myIntPinId = context.FindOrCreatePin(intPin);
		}
	}

	ScriptNodeResult Execute(ScriptExecutionContext& context, ScriptPinId) const override
	{
		ScriptLinkData data = context.ReadInputPin(myIntPinId);
		Print(std::to_string(std::get<int>(data.data)));
		context.TriggerOutputPin(myOutPinId);

		return ScriptNodeResult::Finished;
	}
};

class PrintStringNode : public ScriptNodeBase
{
	ScriptPinId myStringPinId;
	ScriptPinId myOutPinId;

public:
	void Init(const ScriptCreationContext& context) override
	{
		{
			ScriptPin flowInPin = {};
			flowInPin.dataType = ScriptLinkDataType::Flow;
			flowInPin.name = ScriptStringRegistry::RegisterOrGetStringId("Run");
			flowInPin.node = context.GetNodeId();
			flowInPin.role = ScriptPinRole::Input;

			context.FindOrCreatePin(flowInPin);
		}

		{
			ScriptPin flowOutPin = {};
			flowOutPin.dataType = ScriptLinkDataType::Flow;
			flowOutPin.name = ScriptStringRegistry::RegisterOrGetStringId("");
			flowOutPin.node = context.GetNodeId();
			flowOutPin.role = ScriptPinRole::Output;

			myOutPinId = context.FindOrCreatePin(flowOutPin);
		}

		{
			ScriptPin intPin = {};
			intPin.dataType = ScriptLinkDataType::String;
			intPin.name = ScriptStringRegistry::RegisterOrGetStringId("");
			intPin.node = context.GetNodeId();
			intPin.defaultValue = { ScriptStringRegistry::RegisterOrGetStringId("Text to print") };
			intPin.role = ScriptPinRole::Input;

			myStringPinId = context.FindOrCreatePin(intPin);
		}
	}

	ScriptNodeResult Execute(ScriptExecutionContext& context, ScriptPinId) const override
	{

		ScriptLinkData data = context.ReadInputPin(myStringPinId);

		ScriptStringId stringId = std::get<ScriptStringId>(data.data);
		Print(ScriptStringRegistry::GetStringFromStringId(stringId).data());

		context.TriggerOutputPin(myOutPinId);

		return ScriptNodeResult::Finished;
	}
};

void RegisterExampleNodes()
{
	ScriptNodeTypeRegistry::RegisterType<PrintIntNode>("Examples/PrintInt", "Prints an integer");
	ScriptNodeTypeRegistry::RegisterType<PrintStringNode>("Examples/PrintString", "Prints a string");
}
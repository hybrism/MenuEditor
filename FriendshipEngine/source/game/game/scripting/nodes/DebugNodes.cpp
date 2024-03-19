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
			ScriptPin stringPin = {};
			stringPin.dataType = ScriptLinkDataType::String;
			stringPin.name = ScriptStringRegistry::RegisterOrGetStringId("");
			stringPin.node = context.GetNodeId();
			stringPin.defaultValue = { ScriptStringRegistry::RegisterOrGetStringId("Text to print") };
			stringPin.role = ScriptPinRole::Input;

			myStringPinId = context.FindOrCreatePin(stringPin);
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

class PrintVectorNode : public ScriptNodeBase
{
	ScriptPinId myStringPinId;
	ScriptPinId myPositionPinId;
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
			ScriptPin stringPin = {};
			stringPin.dataType = ScriptLinkDataType::String;
			stringPin.name = ScriptStringRegistry::RegisterOrGetStringId("");
			stringPin.node = context.GetNodeId();
			stringPin.defaultValue = { ScriptStringRegistry::RegisterOrGetStringId("Name") };
			stringPin.role = ScriptPinRole::Input;

			myStringPinId = context.FindOrCreatePin(stringPin);
		}

		{
			ScriptPin vectorPin = {};
			vectorPin.dataType = ScriptLinkDataType::Vector3f;
			vectorPin.name = ScriptStringRegistry::RegisterOrGetStringId("Vector");
			vectorPin.node = context.GetNodeId();
			vectorPin.defaultValue.data = Vector3f(0.f, 0.f, 0.f);
			vectorPin.role = ScriptPinRole::Input;

			myPositionPinId = context.FindOrCreatePin(vectorPin);
		}
	}

	ScriptNodeResult Execute(ScriptExecutionContext& context, ScriptPinId) const override
	{
		ScriptLinkData positionData = context.ReadInputPin(myPositionPinId);
		ScriptLinkData stringData = context.ReadInputPin(myStringPinId);
		ScriptStringId stringId = std::get<ScriptStringId>(stringData.data);

		PrintVec3(ScriptStringRegistry::GetStringFromStringId(stringId).data(), std::get<Vector3f>(positionData.data));

		context.TriggerOutputPin(myOutPinId);

		return ScriptNodeResult::Finished;
	}
};

void RegisterExampleNodes()
{
	ScriptNodeTypeRegistry::RegisterType<PrintIntNode>("Debug/PrintInt", "Prints an integer");
	ScriptNodeTypeRegistry::RegisterType<PrintStringNode>("Debug/PrintString", "Prints a string");
	ScriptNodeTypeRegistry::RegisterType<PrintVectorNode>("Debug/PrintVector", "Prints a vector");
}
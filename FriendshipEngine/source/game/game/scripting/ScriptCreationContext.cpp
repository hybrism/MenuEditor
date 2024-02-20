#include "pch.h"

#include "ScriptCreationContext.h"

#include "Script.h"
#include <cassert>


ScriptCreationContext::ScriptCreationContext(Script& script, ScriptNodeId nodeId)
	: myScript(script)
	, myNodeId(nodeId)
{}

ScriptPinId ScriptCreationContext::FindOrCreatePin(ScriptPin pinData) const
{
	assert(pinData.name.id != ScriptStringId::InvalidId);
	assert(pinData.node == myNodeId);

	assert(pinData.role != ScriptPinRole::Input || pinData.dataType != ScriptLinkDataType::Bool || std::holds_alternative<bool>(pinData.defaultValue.data));
	assert(pinData.role != ScriptPinRole::Input || pinData.dataType != ScriptLinkDataType::Int || std::holds_alternative<int>(pinData.defaultValue.data));
	assert(pinData.role != ScriptPinRole::Input || pinData.dataType != ScriptLinkDataType::Float || std::holds_alternative<float>(pinData.defaultValue.data));

	size_t count;
	const ScriptPinId* pins;
	if (pinData.role == ScriptPinRole::Input)
	{
		pins = myScript.GetInputPins(myNodeId, count);
	}
	else
	{
		pins = myScript.GetOutputPins(myNodeId, count);
	}

	for (int i = 0; i < count; i++)
	{
		ScriptPinId id = pins[i];
		const ScriptPin& oldPin = myScript.GetPin(id);
		if (oldPin.name == pinData.name)
		{
			if (oldPin.overridenValue.data.index() == pinData.defaultValue.data.index())
			{
				pinData.overridenValue = oldPin.overridenValue;
			}

			myScript.SetPin(id, pinData);
			return id;
		}
	}

	return myScript.CreatePin(pinData);
}

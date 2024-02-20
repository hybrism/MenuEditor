#include "SetOverriddenValueCommand.h"
#include <cassert>
#include <game/scripting/Script.h>

void SetOverriddenValueCommand::ExecuteImpl()
{
	ScriptPin pin = myScript.GetPin(myPinId);

	assert("New overriden value has different datatype than current value" && pin.defaultValue.data.index() == myNewData.data.index());

	myOldData = pin.overridenValue;
	pin.overridenValue = myNewData;

	myScript.SetPin(myPinId, pin);
}

void SetOverriddenValueCommand::UndoImpl()
{
	ScriptPin pin = myScript.GetPin(myPinId);

	pin.overridenValue = myOldData;

	myScript.SetPin(myPinId, pin);
}
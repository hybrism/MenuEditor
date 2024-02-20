#include "CreateLinkCommand.h"
#include <assert.h>

#include <game/scripting/nodes/ScriptNodeBase.h>
#include <game/scripting/Script.h>

void CreateLinkCommand::ExecuteImpl()
{
	if (myLinkId.id == ScriptLinkId::InvalidId)
	{
		ScriptPinId sourcePinId = myLinkData.sourcePinId;
		ScriptPinId targetPinId = myLinkData.targetPinId;
		const ScriptPin& sourcePin = myScript.GetPin(sourcePinId);
		const ScriptPin& targetPin = myScript.GetPin(targetPinId);

		assert(sourcePin.dataType == targetPin.dataType);
		assert(sourcePin.dataType != ScriptLinkDataType::Unknown);
		assert(sourcePin.role != ScriptPinRole::Input);
		assert(targetPin.role != ScriptPinRole::Output);

		if (sourcePin.dataType == ScriptLinkDataType::Flow)
		{
			size_t count;
			const ScriptLinkId* links = myScript.GetConnectedLinks(sourcePinId, count);
			for (size_t i = 0; i < count; i++)
			{
				myDestroyNodeAndLinksCommand.Add(links[i]);
			}
		}
		else
		{
			size_t count;
			const ScriptLinkId* links = myScript.GetConnectedLinks(targetPinId, count);
			for (size_t i = 0; i < count; i++)
			{
				myDestroyNodeAndLinksCommand.Add(links[i]);
			}
		}

		myDestroyNodeAndLinksCommand.ExecuteImpl();
		myLinkId = myScript.CreateLink(myLinkData);
	}
	else
	{
		myDestroyNodeAndLinksCommand.ExecuteImpl();
		myScript.CreateLinkWithReusedId(myLinkId, myLinkData);
	}
}

void CreateLinkCommand::UndoImpl()
{
	myScript.RemoveLink(myLinkId);
	myDestroyNodeAndLinksCommand.UndoImpl();
}

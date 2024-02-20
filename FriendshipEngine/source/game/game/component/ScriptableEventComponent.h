#pragma once
#include "../utility/Events.h"

struct ScriptableEventComponent : public Component<ScriptableEventComponent>
{
	eid_t triggerEntityID;
	eEvent eventToTrigger;
	bool isInProgress = false;
};

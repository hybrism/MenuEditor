#pragma once
#include <ecs/component/Component.h>
#include <engine/math/Vector.h>
#include "../utility/Events.h"


struct EventComponent : public Component<EventComponent>
{
	eid_t ownerID;
	eid_t targetID;
	eEvent eventToTrigger;
	bool isComplete = false;
	float timer = 0;
	float duration = 2.f;
};

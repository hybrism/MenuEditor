#pragma once
#include <ecs/component/Component.h>
#include <engine/math/Vector.h>
#include "../utility/Events.h"


struct EventComponent : public Component<EventComponent>
{
	eid_t targetID = INVALID_ENTITY;
	eEvent eventToTrigger = eEvent::Count;
	bool isComplete = false;
	float timer = 0;
	float duration = 2.f;
};
#pragma once
#include <ecs\component\Component.h>
#include <engine\math\Vector.h>

struct NpcComponent : public Component<NpcComponent>
{
	
	int ID;
	int GroupID;
	float DelayTimer;
	bool IsActive;


};
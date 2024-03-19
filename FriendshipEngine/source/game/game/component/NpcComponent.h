#pragma once
#include <ecs\component\Component.h>
#include <engine\math\Vector.h>

enum class npcState
{
	dead,
	walk,
	count
};

struct NpcComponent : public Component<NpcComponent>
{
	
	int ID;
	int GroupID;
	float DelayTimer;
	float movementSpeed;
	bool IsActive;
	Vector3f walkToPos;
	Vector3f StartPos;
	Vector3f lerpingPos;



	npcState myNpcState = npcState::dead;
};


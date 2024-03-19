#pragma once
#include <ecs\component\Component.h>
#include <engine\math\Vector.h>


struct DeathZoneComponent : public Component<DeathZoneComponent>
{
	Vector3f overlapPos;
	Vector3f myScale;

};


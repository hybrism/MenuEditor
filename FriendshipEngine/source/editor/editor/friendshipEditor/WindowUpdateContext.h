#pragma once
class World;

struct WindowUpdateContext
{
	float dt;
	World* world;
};
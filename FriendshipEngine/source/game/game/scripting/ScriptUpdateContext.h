#pragma once

class World;

struct ScriptUpdateContext
{
	float deltaTime;
	World* world = nullptr;
};
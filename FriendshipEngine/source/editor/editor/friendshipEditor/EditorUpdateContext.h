#pragma once
class World;
class EditorManager;
class Game;

struct EditorUpdateContext
{
	float dt;
	World* world;
	Game* game;
	EditorManager* editorManager;
};
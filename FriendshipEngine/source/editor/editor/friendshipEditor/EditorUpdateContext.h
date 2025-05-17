#pragma once
class World;
class EditorManager;
class Game;
class FreeCameraHandler;

struct EditorUpdateContext
{
	float dt;
	World* world = nullptr;
	Game* game = nullptr;
	EditorManager* editorManager = nullptr;
	FreeCameraHandler* freeCamHandler = nullptr;
};
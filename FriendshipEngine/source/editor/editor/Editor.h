#pragma once
#include "friendshipEditor/EditorUpdateContext.h"
#include "FreeCameraHandler.h"

class Game;
class EditorManager;
class StateStack;
class GraphicsEngine;

class Editor
{
public:
	Editor();
	~Editor();
	void Init(Game* aGame);
	void Update(const float& dt);
	void Render();


private:
	Game* myGame = nullptr;
	StateStack* myStateStack = nullptr;
	EditorManager* myEditorManager = nullptr;

	FreeCameraHandler myFreeCamHandler;
};
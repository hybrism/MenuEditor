#pragma once

#include "friendshipEditor/EditorUpdateContext.h"
#include <engine/debug/DebugCamera.h>

class Game;
class EditorManager;
class StateStack;

class Editor
{
public:
	Editor();
	~Editor();
	void Init(Game* aGame);
	void Update(const float& dt);
	void Render();


	void SwitchToDebugCamera(float dt);
private:
	Game* myGame = nullptr;
	StateStack* myStateStack = nullptr;
	EditorManager* myEditorManager = nullptr;

	DebugCamera myDebugCamera;
};
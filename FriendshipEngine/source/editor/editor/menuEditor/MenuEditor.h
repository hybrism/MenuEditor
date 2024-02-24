#pragma once

class Game;

class MenuEditor
{
public:
	MenuEditor();
	~MenuEditor();

	void Init(Game* aGame);

	void ViewMenuHandlerWindow();

	Game* myGame;
}; 
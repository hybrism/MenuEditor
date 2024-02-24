#pragma once
#include <string>
#include <imgui/imgui.h>
#include <ecs/system/System.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include <engine/math/Vector.h>
#include "windows/Window.h"
#include "../menuEditor/MenuEditor.h"

#include <engine/Defines.h>

class World;
class Game;

struct GameWindowRect
{
	ImVec2 vMinWin;
	ImVec2 vMaxWin;
};

class EditorManager
{
public:
	EditorManager();
	~EditorManager();

	void Init(Game* aGame);
	void Update(EditorUpdateContext aContext);

	void SetGameWindowRect(const GameWindowRect& aRect) { myGameWindowRect = aRect; }
	GameWindowRect GetGameWindowRect() const { return myGameWindowRect; }
private:
	std::unordered_map<FE::ID, std::shared_ptr<FE::WindowBase>> myWindows; 
	Game* myGame;

	void Dockspace();
	void MenuBar();

	//TODO: Move this to GameWindow
	GameWindowRect myGameWindowRect;
	int myFPS = 0;
	float myFPSTimer = 0.f;
	float myFPSUpdateFrequency = 1.f;
	bool myFirstFrameSetup;
	void ShowDebugData(const float& dt);
	
	//This is used for debug-level selector
	void LoadImportFiles();
	std::vector<std::string> myJsonFileNames;

	//TODO: Test it out and move this to editorUtility?
	void MakeTabVisible(const char* window_name);

	// TODO: Move these to separate classes
	void ViewLevelSelector();
	//void ViewMenuHandlerWindow();
	//MenuEditor myMenuEditor;
};
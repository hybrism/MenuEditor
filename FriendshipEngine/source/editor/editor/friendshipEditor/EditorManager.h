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
	std::vector<std::string> myJsonFileNames;
	GameWindowRect myGameWindowRect;
	Game* myGame;
	bool myFirstFrameSetup;

	void Dockspace();
	void MenuBar();

	//This is used for debug-level selector
	void LoadImportFiles();

	//TODO: Test it out and move this to editorUtility?
	void MakeTabVisible(const char* window_name);

};
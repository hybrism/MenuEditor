#pragma once
#include <thread>
#include "scene/SceneManager.h"
#include <engine/graphics/PostProcess.h>
#include "engine\graphics\Light\LightManager.h"
#include "engine\graphics\Light\SkyBox.h"

class Game
{
public:
	Game();
	~Game();
	void Init();
	void Update(const float& dt);
	void Render();

	SceneManager& GetSceneManager() { return mySceneManager; }
	LightManager& GetLightManager() { return myLightManager; }
	PostProcess& GetPostProcess() { return myPostProcess; }
private:
	std::thread myAssetLoadingThread = {};

	SceneManager mySceneManager;
	PostProcess myPostProcess;
	LightManager myLightManager;
	SkyBox mySkyBox;
	SceneUpdateContext myContext;
};
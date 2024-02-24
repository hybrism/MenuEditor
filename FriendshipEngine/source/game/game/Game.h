#pragma once
#include <thread>
#include "scene/SceneManager.h"
#include <engine/graphics/PostProcess.h>
#include "engine\graphics\Light\LightManager.h"


class Game
{
public:
	Game();
	~Game();
	void Init();
	void Update(const float& dt);
	void Render();

	SceneManager& GetSceneManager() { return mySceneManager; }

private:
	std::thread myAssetLoadingThread = {};

	SceneManager mySceneManager;
	PostProcess myPostProcess;
	LightManager myLightManager;
};
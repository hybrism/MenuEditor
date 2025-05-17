#pragma once

class SceneManager;
class LightManager;
class PostProcess;
class VFXManager;
class ParticleSystemManager;
class Game;
class GameTimer;

namespace MENU
{
	class MenuHandler;
}

struct SceneUpdateContext
{
	float dt;
	double totalTime;
	PostProcess* postProcess;
	LightManager* lightManager;
	SceneManager* sceneManager;
	VFXManager* vfxManager;
	ParticleSystemManager* particleSystemManager;
	GameTimer* gameTimer;
	MENU::MenuHandler* menuHandler;
};

enum class eSceneType
{
	Game,
	MainMenu,
	Count
};

enum class eLevel
{
	Lvl0 = 0,
	Lvl1 = 1,
	Lvl2 = 2,
	Lvl3 = 3,
	FeatureGym,
	None,
	Count
};

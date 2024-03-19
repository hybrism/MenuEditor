#pragma once
class SceneManager;
class LightManager;
class PostProcess;
class Game;

struct SceneUpdateContext
{
	float dt;
	PostProcess* postProcess;
	LightManager* lightManager;
	SceneManager* sceneManager;
};

enum class eSceneType
{
	Game,
	MainMenu,
	Count
};

enum class eLevel
{
	None,
	FeatureGym,
	Lvl0_Tutorial,
	Lvl1,
	Lvl2,
	Lvl3_FamilyHeirloom,
	AssetGym,

	AxelFeatureGym,
	Count
};

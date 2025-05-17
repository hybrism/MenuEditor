#pragma once
#include <thread>

#include <engine/graphics/PostProcess.h>
#include <engine/graphics/Light/LightManager.h>
#include <engine/graphics/Light/SkyBox.h>
#include <engine/graphics/vfx/VFXManager.h>
#include <engine/graphics/vfx/ParticleSystemManager.h>

#include "scene/SceneManager.h"

class Game
{
public:
	Game();
	~Game();
	void Init();
	void Update(const float& dt, const double& aTotalTime);
	void Render();

	SceneManager& GetSceneManager() { return mySceneManager; }
	LightManager& GetLightManager() { return myLightManager; }
	PostProcess& GetPostProcess() { return myPostProcess; }
	VFXManager& GetVFXManager() { return myVfxManager; }
	ParticleSystemManager& GetParticleSystemManager() { return myParticleSystemManager; }

private:
	LightManager myLightManager;
	SceneManager mySceneManager;
	PostProcess myPostProcess;
	VFXManager myVfxManager;
	SceneUpdateContext myContext;
	SkyBox mySkyBox;

	ParticleSystemManager myParticleSystemManager;
};
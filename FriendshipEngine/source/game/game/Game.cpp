#include "pch.h"
#include "Game.h"
#include <engine/graphics/Camera.h>
#include <engine/Defines.h>

#include <assets/ModelFactory.h>
#include <PhysX/PxPhysicsAPI.h>

#include <engine/utility/InputManager.h>

#include <assets/AssetDatabase.h>
#include <engine/graphics/PostProcess.h>
#include <engine/graphics/renderer/DeferredRenderer.h>
#include <engine/graphics/renderer/ForwardRenderer.h>
#include <engine/graphics/Light/LightManager.h>
#include <engine/graphics/vfx/VFXManager.h>
#include <engine/graphics/vfx/VisualEffectFactory.h>
#include <engine/graphics/vfx/VFXDatabase.h>


//#include "audio/AudioManager.h"
#include "audio/NewAudioManager.h"
//#include "audio/FMODImplementation.h"


Game::Game() : mySceneManager(), myPostProcess(), myParticleSystemManager(), myVfxManager(myParticleSystemManager)
{
	auto camera = GraphicsEngine::GetInstance()->GetCamera();
	camera->GetTransform().SetPosition({ 0, 50, -75.0f });

	//NewAudioManager::GetInstance()->Create();
	NewAudioManager::GetInstance()->Init();


}

Game::~Game()
{
	//AudioManager::GetInstance()->Destroy();
	AssetDatabase::DestroyInstance();
}

void Game::Init()
{
	AssetDatabase::LoadAssetsFromJson("Resources.json");

	myContext.lightManager = &GetLightManager();
	myContext.postProcess = &GetPostProcess();
	myContext.sceneManager = &GetSceneManager();
	myContext.vfxManager = &GetVFXManager();
	myContext.particleSystemManager = &GetParticleSystemManager();

	myPostProcess.Init();
	mySceneManager.Init(myContext);
	myLightManager.Init({ 0,-1,0 }, { 118.f / 256.f, 75.f / 256.f, 35.f / 256.f ,1 });
	mySkyBox.Init();
}

void Game::Update(const float& aDT, const double& aTotalTime)
{
	myContext.dt = aDT;
	myContext.totalTime = aTotalTime;

	if (!mySceneManager.Update(myContext))
		PostQuitMessage(0);

	NewAudioManager::GetInstance()->Update();

	myVfxManager.Update(aDT);
	myParticleSystemManager.Update(aDT);


	if (InputManager::GetInstance()->IsKeyPressed('H'))
	{
		NewAudioManager::GetInstance()->PlaySound(eSounds::Jump, 1.0f);
	}
	
	//NewAudioManager::GetInstance()->Update();

	mySceneManager.LateUpdate(myContext);
}

void Game::Render()
{
	mySceneManager.Render();
	myVfxManager.Render();
	myParticleSystemManager.Render();

	{
		GraphicsEngine* ge = GraphicsEngine::GetInstance();
		GBuffer& gBuffer = ge->GetGBuffer();
		DeferredRenderer& deferred = ge->GetDeferredRenderer();
		ForwardRenderer& forward = ge->GetForwardRenderer();
		SpriteRenderer& sprite = ge->GetSpriteRenderer();

		auto prevDrawCalls = ge->GetDrawCalls();

		// Shadows
		myLightManager.BeginShadowRendering();
		deferred.DoShadowRenderPass();
		myLightManager.EndShadowRendering();

		if ((ge->GetDrawCalls() - prevDrawCalls) > 0)
		{
			deferred.DoGBufferPass();

#ifndef _RELEASE
			myLightManager.RenderPointLightSpheres();
#endif
			myPostProcess.FirstFrame();

			{
				mySkyBox.Render(); //TODO: Find a nicer place for the SkyBox to live
				gBuffer.SetAllAsResources(1);
				myLightManager.LightRender();
				myLightManager.SetResourcesOnSlot();


				deferred.DoFinalPass();
				forward.DoRenderPass();
			}

			myLightManager.ClearAllResources();
		}
		myPostProcess.Render();

#ifdef _EDITOR
		ge->GetDebugRenderer().Render();
#endif // _EDITOR

		sprite.Render();

	}
}

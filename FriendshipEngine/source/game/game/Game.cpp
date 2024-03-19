#include "pch.h"
#include "Game.h"
#include <engine/graphics/Camera.h>
#include <engine/Defines.h>

#include <assets/ModelFactory.h>
#include <PhysX\PxPhysicsAPI.h>

#include <engine/utility/InputManager.h>

#include <assets/AssetDatabase.h>
#include <engine/graphics/PostProcess.h>
#include <engine/graphics/renderer/DeferredRenderer.h>
#include <engine/graphics/renderer/ForwardRenderer.h>
#include <engine\graphics\Light\LightManager.h>
//#include "audio/AudioManager.h"
//#include "audio/FMODImplementation.h"

Game::Game() : mySceneManager(), myPostProcess()
{
	auto camera = GraphicsEngine::GetInstance()->GetCamera();
	camera->GetTransform().SetPosition({ 0, 50, -75.0f });

	AssetDatabase::CreateInstance();
	//AudioManager::GetInstance()->Create();
	//AudioManager::GetInstance()->Init();
}

Game::~Game()
{
	//AudioManager::GetInstance()->Destroy();
	AssetDatabase::DestroyInstance();

	if (myAssetLoadingThread.joinable())
	{
		myAssetLoadingThread.join();
	}
}

void Game::Init()
{
	myAssetLoadingThread = std::thread([&]
		{
			AssetDatabase::LoadAssetsFromJson("Resources.json");
		});

	while (!AssetDatabase::HasLoadedAssets()) {}

	myPostProcess.Init();
	mySceneManager.Init();
	myLightManager.Init({ 0,-1,0 }, { 118.f / 256.f, 75.f / 256.f, 35.f / 256.f ,1 });
	mySkyBox.Init();

	myContext.lightManager = &GetLightManager();
	myContext.postProcess = &GetPostProcess();
	myContext.sceneManager = &GetSceneManager();
}

void Game::Update(const float& aDT)
{
	myContext.dt = aDT;
	if (!mySceneManager.Update(myContext))
		PostQuitMessage(0);

	mySceneManager.LateUpdate();
}

void Game::Render()
{
	mySceneManager.Render();

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
		forward.DoShadowRenderPass();
		myLightManager.EndShadowRendering();



		if ((ge->GetDrawCalls() - prevDrawCalls) > 0)
		{
			deferred.DoGBufferPass();
			//mySkyBox.Render(); //TO:DO Find a nicer place for the SkyBox to live.  Utkommenterad tills jag vet varf?r vingarna f?rsvinner n?r man titttar p? skyboxen



#ifndef _RELEASE
			myLightManager.RenderPointLightSpheres();
#endif


			myPostProcess.FirstFrame();
			{
				gBuffer.SetAllAsResources(1);

				myLightManager.LightRender();
				myLightManager.SetResourcesOnSlot();

				deferred.DoFinalPass();
				forward.DoRenderPass();
			}


			myLightManager.ClearAllResources();
		}
		myPostProcess.Render();
		sprite.Render();
	}
}

//void Game::Clear()
//{
//	auto* ge = GraphicsEngine::GetInstance();
//	auto* context = ge->DX().GetContext();
//	ID3D11ShaderResourceView* nullsrv = nullptr;
//	context->PSSetShaderResources(13, 1, &nullsrv);
//}

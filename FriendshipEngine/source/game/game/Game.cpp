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
#include <engine/graphics/DirectionalLightManager.h>

Game::Game() : mySceneManager(), myPostProcess()
{
	auto camera = GraphicsEngine::GetInstance()->GetCamera();
	camera->SetPosition({ 0, 50, -75.0f });

	AssetDatabase::CreateInstance();
}

Game::~Game()
{
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
}

void Game::Update(const float& dt)
{
	if (!mySceneManager.Update(dt))
		PostQuitMessage(0);

	mySceneManager.LateUpdate();
}

void Game::Render()
{
	mySceneManager.Render();

	{
		GraphicsEngine* ge = GraphicsEngine::GetInstance();
		DeferredRenderer& deferred = ge->GetDeferredRenderer();
		ForwardRenderer& forward = ge->GetForwardRenderer();

		// Shadows
		deferred.DoShadowRenderPass();
		forward.DoShadowRenderPass();

		// GBuffer before PostProcess is applied (different RenderTarget)
		deferred.DoGBufferPass();

		// PostProcessing
		myPostProcess.FirstFrame();
		{
			deferred.DoFinalPass();
			forward.DoRenderPass();
		}
		myPostProcess.Render();
	}
}

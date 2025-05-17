#include "pch.h"

#include "GameApplication.h"

#include <engine/utility/Error.h>
#include <engine/Engine.h>
#include <engine/utility/StringHelper.h>
#include <engine/utility/InputManager.h>
#include "../Game.h"

#include "gui/MenuHandler.h"
#include "gui/MenuObject.h"
#include "gui/components/SpriteComponent.h"

#include "scripting/nodes/RegisterNodeFunctions.h"

LRESULT GameApplication::WindowProcedure(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	if (IsWindowEnabled(hWnd))
	{
		if (InputManager::GetInstance()->UpdateEvents(uMsg, wParam, lParam))
			return 0;
	}

	return ::Application::WindowProcedure(hWnd, uMsg, wParam, lParam);
}

GameApplication::GameApplication() : Application()
{
	myGame = nullptr;
	myEngine = Engine::CreateInstance(&myTimer);
}

GameApplication::~GameApplication()
{
	if (myGameInitThread.joinable())
	{
		myGameInitThread.join();
	}
	InputManager::GetInstance()->DestroyInstance();

	myEngine->DestroyInstance();
	myEngine = nullptr;

	delete myGame;
	myGame = nullptr;
}

bool GameApplication::BeginFrame()
{
	return myEngine->BeginFrame();
}

void GameApplication::Init(HINSTANCE hInstance, WNDPROC wndProc)
{
	Vector2i resolution = {};
	std::wstring icon;

	{
		std::vector<RECT> monitorRects;
		EnumDisplayMonitors(NULL, NULL, [](HMONITOR, HDC, LPRECT lprcMonitor, LPARAM dwData)->BOOL {
			std::vector<RECT>* monitorRects = reinterpret_cast<std::vector<RECT>*>(dwData);
			monitorRects->push_back(*lprcMonitor);
			return TRUE;
			}, reinterpret_cast<LPARAM>(&monitorRects));

		RECT primaryMonitorRect = monitorRects[0];
		resolution.x = primaryMonitorRect.right - primaryMonitorRect.left;
		resolution.y = primaryMonitorRect.bottom - primaryMonitorRect.top;
	}
#ifndef _RELEASE
	//resolution.x = static_cast<int>(static_cast<float>(resolution.x) * 0.8f);
	//resolution.y = static_cast<int>(static_cast<float>(resolution.y) * 0.8f);
	icon = std::wstring(StringHelper::s2ws(RELATIVE_EDITOR_ASSET_PATH) + L"icon.ico");
#else
	icon = L"icon.ico";
#endif

	// TODO: Read this from a config file

	const wchar_t* ICON_PATH = icon.c_str();

	if (!myEngine->Init(resolution.x, resolution.y, L"Engine", L"FriendshipEngine", ICON_PATH, hInstance, wndProc))
	{
		PrintE("Engine could not Initiate!");
		return;
	}

#ifndef _RELEASE
	WELCOME_TO_FRIENDSHIPENGINE
#endif


	//Register Nodes
	RegisterExampleNodes();
	RegisterCommonNodes();
	RegisterEntityNodes();
	RegisterCollisionNodes();
	RegisterMathNodes();

	AssetDatabase::CreateInstance();

	myGameInitThread = std::thread([&]
		{
			myGame = new Game();
			myGame->Init();
			myHasInitedGame = true;
		});

	//TOVES Ugly fix to be able to play in Editor
#ifdef _EDITOR
	while (!myHasInitedGame)
	{
		Print("Initing Game...");
	}
#endif

#ifdef _RELEASE
	//INIT SPLASH SCREEN
	{
		Vector2i screenSizeui = GraphicsEngine::GetInstance()->DX().GetViewportDimensions();
		Vector2f screenCenter = { (float)screenSizeui.x * 0.5f, (float)screenSizeui.y * 0.5f };
		mySplashScreen.guiElements = new MENU::MenuHandler();
		mySplashScreen.guiElements->Init("splashScreen.json");

		{
			//TGA LOGO
			MENU::MenuObject& tgaLogo = mySplashScreen.guiElements->CreateNewObject(screenCenter);
			mySplashScreen.tgaLogoId = tgaLogo.GetID();
			MENU::SpriteComponent& sprite = tgaLogo.AddComponent<MENU::SpriteComponent>();
			sprite.SetTexture(AssetDatabase::GetTextureDatabase().GetOrLoadSpriteTexture("s_tga_logo_white.dds"), "s_tga_logo_white.dds");
			sprite.SetScaleMultiplier({ 0.65f, 0.65f });
		}

		{
			//SKIPPER LOGO
			MENU::MenuObject& groupLogo = mySplashScreen.guiElements->CreateNewObject(screenCenter);
			mySplashScreen.groupLogoId = groupLogo.GetID();
			MENU::SpriteComponent& sprite = groupLogo.AddComponent<MENU::SpriteComponent>();
			sprite.SetTexture(AssetDatabase::GetTextureDatabase().GetOrLoadSpriteTexture("s_skipperLogo.dds"), "s_skipperLogo.dds");
			sprite.SetScaleMultiplier({ 0.25f, 0.25f });
			sprite.SetIsHidden(true);
		}

	}
#endif
}

void GameApplication::Update(const float& dt, const double& aTotalTime)
{
	::Application::Update(dt, aTotalTime);

#ifdef _RELEASE
	if (!mySplashScreen.isDone)
	{
		UpdateSplashScreen(dt);
		mySplashScreen.guiElements->Update({});
		mySplashScreen.guiElements->Render();

		GraphicsEngine::GetInstance()->GetSpriteRenderer().Render();

		return;
	}
#endif

	if (!myHasInitedGame)
	{
		myGameInitThread.join();
		return;
	}

#ifndef _RELEASE
	auto* im = InputManager::GetInstance();
	if (im->IsKeyPressed('0'))
	{
		myShouldStep = !myShouldStep;
	}

	if (myShouldStep)
	{
		if (im->IsKeyPressed('9'))
		{
			float multiplier = 1.0f;
			if (im->IsKeyHeld(VK_SHIFT))
				multiplier = 10.0f;
			myGame->Update(0.016f * multiplier, aTotalTime);
		}
	}
	else
	{
		myGame->Update(dt, aTotalTime);
	}
#else
	myGame->Update(dt, aTotalTime);
#endif
	InputManager::GetInstance()->Update();
}

void GameApplication::Render()
{
#ifdef _RELEASE
	if (!mySplashScreen.isDone) { return; }
#endif

	if (!myHasInitedGame) { return; }

	myGame->Render();
}

bool GameApplication::UpdateSplashScreen(const float& dt)
{
	if (dt > 2.f)
		return false;

	mySplashScreen.timer += dt;

	if (!mySplashScreen.hasChangedSprite && mySplashScreen.timer >= mySplashScreen.fullDuration / 2.f)
	{
		MENU::MenuObject& tgaLogo = mySplashScreen.guiElements->GetObjectFromID(mySplashScreen.tgaLogoId);
		tgaLogo.GetComponent<MENU::SpriteComponent>().SetIsHidden(true);

		MENU::MenuObject& skipperLogo = mySplashScreen.guiElements->GetObjectFromID(mySplashScreen.groupLogoId);
		skipperLogo.GetComponent<MENU::SpriteComponent>().SetIsHidden(false);

		mySplashScreen.hasChangedSprite = true;
	}

	if (mySplashScreen.timer >= mySplashScreen.fullDuration)
		mySplashScreen.isDone = true;

	return mySplashScreen.isDone;
}

void GameApplication::EndFrame()
{
	::Application::EndFrame();
	myEngine->EndFrame();
}

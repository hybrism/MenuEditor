#include "pch.h"

#include "GameApplication.h"

#include <engine/utility/Error.h>
#include <engine/Engine.h>
#include <engine/utility/StringHelper.h>
#include <engine/utility/InputManager.h>
#include "../Game.h"

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
	resolution.x = static_cast<int>(static_cast<float>(resolution.x) * 0.8f);
	resolution.y = static_cast<int>(static_cast<float>(resolution.y) * 0.8f);
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

	myGame = new Game();
	myGame->Init();

}

void GameApplication::Update(const float& dt)
{
	::Application::Update(dt);
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
			myGame->Update(0.016f * multiplier);
		}
	}
	else
	{
		myGame->Update(dt);
	}
#else
	myGame->Update(dt);
#endif
	InputManager::GetInstance()->Update();
}

void GameApplication::Render()
{
	myGame->Render();
}

void GameApplication::EndFrame()
{
	::Application::EndFrame();
	myEngine->EndFrame();
}

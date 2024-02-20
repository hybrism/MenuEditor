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

#ifdef _DEBUG
	resolution = { 1600, 900 };
	icon = std::wstring(StringHelper::s2ws(RELATIVE_EDITOR_ASSET_PATH) + L"icon.ico");
#else
	resolution = { 1920, 1080 };
	icon = L"icon.ico";
#endif

	// TODO: Read this from a config file

	const wchar_t* ICON_PATH = icon.c_str();

	if (!myEngine->Init(resolution.x, resolution.y, L"Engine", L"FriendshipEngine", ICON_PATH, hInstance, wndProc))
	{
		PrintE("Engine could not Initiate!");
		return;
	}

#ifdef _DEBUG
	WELCOME_TO_FRIENDSHIPENGINE
#endif

	//Register Nodes
	RegisterExampleNodes();
	RegisterCommonNodes();


	myGame = new Game();
	myGame->Init();

}

void GameApplication::Update(const float& dt)
{
	::Application::Update(dt);
	myGame->Update(dt);
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

#include "pch.h"

#include "Engine.h"
#include "Defines.h"
#include "graphics/GraphicsEngine.h"
#include "graphics/Camera.h"
#include "utility/InputManager.h"
#include "utility/Error.h"

#include "Paths.h"

#define WINDOWED_STYLE WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX
//#define FULLSCREEN_STYLE WS_POPUP

Engine* Engine::myInstance = nullptr;

Engine* Engine::CreateInstance(Timer* aTimer)
{
	if (myInstance == nullptr)
	{
		myInstance = new Engine(aTimer);
	}
	return myInstance;
}

Engine* Engine::GetInstance()
{
	return myInstance;
}

void Engine::DestroyInstance()
{
	if (myInstance != nullptr)
	{
		delete myInstance;
		myInstance = nullptr;
	}
}

Engine::Engine(Timer* aTimer)
{
	myTimer = aTimer;
	myGraphicsEngine = GraphicsEngine::GetInstance();
	myShouldExit = false;
	myInputManager = InputManager::CreateInstance();
}

HCURSOR Engine::LoadCustomCursor(LPCWSTR aCurFileName)
{
	HICON icon = (HICON)LoadImage(nullptr, aCurFileName, IMAGE_CURSOR, 32, 32, LR_LOADFROMFILE | LR_SHARED);
	ICONINFO cur;
	GetIconInfo(icon, &cur);
	cur.xHotspot = 0;
	cur.yHotspot = 0;

	HCURSOR hCursor = CreateIconIndirect(&cur);
	if (!hCursor)
	{
		PrintW("Could not load custom cursor!");
	}

	return hCursor;
}

Engine::~Engine()
{
	GraphicsEngine::GetInstance()->DestroyInstance();
	InputManager::DestroyInstance();
}

void Engine::SetResolution(const int aWidth, const int aHeight)
{
	SetWindowDimensions(myWindowPositionX, myWindowPositionY, aWidth, aHeight, myIsFullscreen);
}

void Engine::SetWindowDimensions(const int aX,
	const int aY,
	int aWidth,
	int aHeight,
	const bool aFullscreen
)
{
	aWidth = aWidth == -1 ? myWindowWidth : aWidth;
	aHeight = aHeight == -1 ? myWindowHeight : aHeight;

	RECT wr = {};
	wr.left = aX;
	wr.right = aWidth + wr.left;
	wr.top = aY;
	wr.bottom = aHeight + wr.top;

	myIsFullscreen = aFullscreen;

	//if (aFullscreen)
	//{
	//	SetWindowLong(hWnd, GWL_STYLE, FULLSCREEN_STYLE);
	//}
	//else
	{
		SetWindowLong(hWnd, GWL_STYLE, WINDOWED_STYLE);
		AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);
	}

	aWidth = wr.right - wr.left;
	aHeight = wr.bottom - wr.top;

	//adjust window size to account for window borders
	myWindowWidth = aWidth;
	myWindowHeight = aHeight;
	myWindowPositionX = aX;
	myWindowPositionY = aY;
	SetWindowPos(hWnd, nullptr, aX, aY, aWidth, aHeight, SWP_NOZORDER);
}

bool Engine::Init(
	const int& aWindowWidth,
	const int& aWindowHeight,
	const wchar_t* aClassName,
	const wchar_t* aWindowName,
	const wchar_t* aIconPath,
	HINSTANCE& hInstance,
	WNDPROC WndProc
)
{

	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = (HICON)LoadImage(nullptr, aIconPath, IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);

#ifdef _EDITOR
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
#else
	wcex.hCursor = LoadCustomCursor(L"cursor.cur");
#endif

	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszClassName = aClassName;
	RegisterClassExW(&wcex);

	myWindowWidth = aWindowWidth;
	myWindowHeight = aWindowHeight;
	myViewPortWidth = aWindowWidth;
	myViewPortHeight = aWindowHeight;
	myWindowPositionX = 0;
	myWindowPositionY = 0;

#ifndef _RELEASE
#if WINDOWED_FULLSCREEN
	//TODO: Instead of magic number, find size of available sceen size?
	unsigned int titlebarSize = GetSystemMetrics(SM_CXMENUSIZE) + 30;

	myViewPortWidth = GetSystemMetrics(SM_CXSCREEN);
	myViewPortHeight = GetSystemMetrics(SM_CYSCREEN) - titlebarSize;
#endif //!WINDOWED_FULLSCREEN
	auto style = WINDOWED_STYLE;
#else
	myIsFullscreen = true;
	auto style = WINDOWED_STYLE;
#endif //!_RELEASE
	hWnd = CreateWindow(
		aClassName,
		aWindowName,
		style,
		myWindowPositionX,
		myWindowPositionY,
		myViewPortWidth,
		myViewPortHeight,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (!hWnd) { return false; }

	InputManager::GetInstance()->Init();

	// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow

#ifndef _RELEASE

#if WINDOWED_FULLSCREEN
	ShowWindow(hWnd, SW_SHOWMAXIMIZED);
#else
	ShowWindow(hWnd, SW_SHOWMAXIMIZED);
#endif //!WINDOWED_FULLSCREEN

#else
	myIsFullscreen = true;
	ShowWindow(hWnd, SW_SHOWDEFAULT);
#endif//!_RELEASE

	UpdateWindow(hWnd);

	//system("mode con COLS=700");
	//ShowWindow(hWnd, SW_SHOW);
	//SendMessage(hWnd, WM_SYSKEYDOWN, VK_RETURN, 0x20000000);

	if (!myGraphicsEngine->Initialize(myWindowWidth, myWindowHeight, hWnd)) { return false; }

	return true;

}

bool Engine::BeginFrame()
{
	if (myShouldExit) { return false; }

	MSG msg = { 0 };
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT)
		{
			myShouldExit = true;
		}
	}

	myGraphicsEngine->BeginFrame();

	return true;
}


void Engine::EndFrame()
{
	myGraphicsEngine->EndFrame();
}

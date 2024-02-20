#include "pch.h"

#include "Engine.h"
#include "Defines.h"
#include "graphics/GraphicsEngine.h"
#include "graphics/Camera.h"
#include "utility/InputManager.h"
#include "utility/Error.h"

#include "Paths.h"

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
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszClassName = aClassName;
	RegisterClassExW(&wcex);

	myWindowWidth = aWindowWidth;
	myWindowHeight = aWindowHeight;
	myViewPortWidth = aWindowWidth;
	myViewPortHeight = aWindowHeight;

#ifdef _DEBUG
#if EDITOR_VIEWPORT_SIZE
	//TODO: Instead of magic number, find size of available sceen size?
	unsigned int titlebarSize = GetSystemMetrics(SM_CXMENUSIZE) + 30;

	myViewPortWidth = GetSystemMetrics(SM_CXSCREEN);
	myViewPortHeight = GetSystemMetrics(SM_CYSCREEN) - titlebarSize;
#endif
	hWnd = CreateWindow(
		aClassName,
		aWindowName,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX,
		0,
		0,
		myViewPortWidth,
		myViewPortHeight,
		nullptr,
		nullptr,
		hInstance,
		nullptr);
#else
	hWnd = CreateWindow(
		aClassName,
		aWindowName,
		WS_POPUP, // Use WS_POPUP style for fullscreen borderless
		0, 0,     // Set position to (0, 0)
		myWindowWidth,
		myWindowHeight,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);
#endif
	if (!hWnd) { return false; }

	InputManager::GetInstance()->Init();

	// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow
#ifdef _DEBUG
	ShowWindow(hWnd, SW_SHOWMAXIMIZED);
#else
	ShowWindow(hWnd, SW_SHOWDEFAULT);
#endif
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

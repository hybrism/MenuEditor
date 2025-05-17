#pragma once
#include <windows.h>

class GraphicsEngine;
class Timer;
class InputManager;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class Engine
{
public:
	static Engine* CreateInstance(Timer* aTimer);
	static Engine* GetInstance();
	static void DestroyInstance();
	~Engine();

	Timer* const GetTimer() { return myTimer; }
	const HWND& GetWindowHandle() { return hWnd; }

	void SetResolution(const int aWidth, const int aHeight);
	void SetWindowDimensions(
		const int aX,
		const int aY,
		int aWidth,
		int aHeight,
		const bool aFullscreen = true
	);

	bool Init(
		const int& aWindowWidth,
		const int& aWindowHeight,
		const wchar_t* aClassName,
		const wchar_t* aWindowName,
		const wchar_t* aIconPath,
		HINSTANCE& hInstance,
		WNDPROC WndProc
	);
	bool BeginFrame();
	void EndFrame();

	bool myIsFullscreen = false;
private:
	Engine(const Engine&) = delete;
	Engine(Timer* aTimer);
	Engine() = delete;

	bool myShouldExit = false;
	int myWindowWidth = 0;
	int myWindowHeight = 0;
	int myViewPortWidth = 0;
	int myViewPortHeight = 0;
	int myWindowPositionX = 0;
	int myWindowPositionY = 0;
	HWND hWnd = {};

	Timer* myTimer = nullptr;
	InputManager* myInputManager = nullptr;
	GraphicsEngine* myGraphicsEngine = nullptr;
	static Engine* myInstance;

	HCURSOR LoadCustomCursor(LPCWSTR aCurFileName);
};
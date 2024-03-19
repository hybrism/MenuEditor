#pragma once

#pragma warning(disable : 4996)

#include "Application.h"
#include "../engine/Defines.h"

extern Application* CreateApplication();
extern LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

#include <Windows.h>

//#ifdef PLATFORM_WINDOWS

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	nCmdShow;

#ifndef _RELEASE
	//Properties -> Preprocessor -> Preprocessor Definitions -> _CRT_SECURE_NO_WARNINGS
#if SHOW_CONSOLE_WINDOW
	AllocConsole(); // Create a new console window
	// Redirect the standard input, output, and error streams to the new console window
	FILE* f;
	f = freopen("CONIN$", "r", stdin);
	f = freopen("CONOUT$", "w", stdout);
	f = freopen("CONOUT$", "w", stderr);
#endif // SHOW_CONSOLE_WINDOW

#endif // _DEBUG

	// Main message loop:
	MSG msg = {};

	auto app = CreateApplication();
	app->Init(hInstance, WndProc);
	app->Run();

	delete app;

	return (int)msg.wParam;
}

#include "Application.h"

#include <iostream>

LRESULT Application::WindowProcedure(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SIZE:
		//HANDE RESIZE
		break;
	case WM_DESTROY:
	case WM_CLOSE:
	case WM_QUIT:
		PostQuitMessage(0);
		break;
	case WM_MOUSEACTIVATE:
		// https://github.com/microsoft/DirectXTK/wiki/Mouse
		// When you click to activate the window, we want Mouse to ignore that event.
		return MA_ACTIVATEANDEAT;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;

}

Application::Application() : myTimer() {}
Application::~Application() {}

void Application::Run()
{
	while (BeginFrame())
	{
		Update(GetTimer().GetDeltaTime(),GetTimer().GetTotalTime());
		Render();
		EndFrame();
	}
}

void Application::EndFrame()
{
	myTimer.Update();
}


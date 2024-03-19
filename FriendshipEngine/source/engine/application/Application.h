#pragma once

#include <Windows.h>
#include "Timer.h"

class Application {
public:
	Application();
	virtual ~Application();

	void Run();
	virtual void Init(HINSTANCE hInstance, WNDPROC wndProc) = 0;
	virtual void Update(const float&) { __noop; }
	virtual void Render() { __noop; }

	static LRESULT WindowProcedure(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
	virtual const Timer& GetTimer() const { return myTimer; }

	virtual bool BeginFrame() = 0;
	virtual void EndFrame();
protected:
	Timer myTimer;
};

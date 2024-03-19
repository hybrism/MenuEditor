#pragma once
#include <application/Application.h>

class Engine;
class Game;

class GameApplication : public Application {
public:
	GameApplication();
	~GameApplication() override;

	void Init(HINSTANCE hInstance, WNDPROC wndProc) override;
	void Update(const float& dt) override;
	void Render() override;

	bool BeginFrame() override;
	void EndFrame() override;

	static LRESULT WindowProcedure(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
	Game* GetGame() { return myGame; }
private:
	Engine* myEngine = nullptr;
	Game* myGame = nullptr;
#ifndef _RELEASE
	bool myShouldStep = false;
#endif
};
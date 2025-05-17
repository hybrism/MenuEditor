#pragma once
#include <thread>
#include <application/Application.h>

class Engine;
class Game;

namespace MENU
{
	class MenuHandler;
}

struct SplashScreen
{
	MENU::MenuHandler* guiElements;
	bool isDone = false;
	bool hasChangedSprite = false;
	float timer = 0.f;
	float fullDuration = 10.f;

	unsigned int groupLogoId = 0;
	unsigned int tgaLogoId = 0;

};

class GameApplication : public Application {
public:
	GameApplication();
	~GameApplication() override;

	void Init(HINSTANCE hInstance, WNDPROC wndProc) override;
	void Update(const float& dt, const double& aTotalTime) override;
	void Render() override;

	bool UpdateSplashScreen(const float& dt);

	bool BeginFrame() override;
	void EndFrame() override;

	static LRESULT WindowProcedure(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
	Game* GetGame() { return myGame; }

private:
	Engine* myEngine = nullptr;
	Game* myGame = nullptr;

	std::thread myGameInitThread = {};
	
	SplashScreen mySplashScreen;

	volatile bool myHasInitedGame = false;

#ifndef _RELEASE
	bool myShouldStep = false;
#endif
};
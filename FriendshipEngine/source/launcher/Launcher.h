#pragma once
#include <application/Application.h>

class GameApplication;

class Launcher : public Application {
public:
	Launcher();
	~Launcher() override;

	void Init(HINSTANCE hInstance, WNDPROC wndProc) override;
	void Update(const float& dt) override;
	void Render() override;
	const Timer& GetTimer() const override;
private:
	void RenderBackbuffer();
	bool BeginFrame() override;
	void EndFrame() override;
	GameApplication* myGameApplication;
};
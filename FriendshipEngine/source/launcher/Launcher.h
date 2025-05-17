#pragma once
#include <application/Application.h>

class GameApplication;

class Launcher : public Application {
public:
	Launcher();
	~Launcher() override;

	void Init(HINSTANCE hInstance, WNDPROC wndProc) override;
	void Update(const float& dt, const double& aTotalTime) override;
	void Render() override;
	const Timer& GetTimer() const override;
	bool BeginFrame() override;
	void EndFrame() override;
private:
	void RenderBackbuffer();
	GameApplication* myGameApplication;
};
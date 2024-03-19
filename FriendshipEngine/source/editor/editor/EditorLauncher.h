#pragma once

#include <application/Application.h>

class GameApplication;
class Editor;

class EditorLauncher : public Application {
public:
	EditorLauncher();
	~EditorLauncher() override;

	void Init(HINSTANCE hInstance, WNDPROC wndProc) override;
	void Update(const float& dt) override;
	void Render();
	const Timer& GetTimer() const override;
	bool BeginFrame() override;
	void EndFrame() override;
	GameApplication* GetGameApplication() { return myGameApplication; };
private:
	Editor* myEditor;
	GameApplication* myGameApplication;
};
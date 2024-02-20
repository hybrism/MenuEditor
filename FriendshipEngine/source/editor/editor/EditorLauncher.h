#pragma once
#include <application/Application.h>

class GameApplication;
class Editor;

class EditorLauncher : public Application {
public:
	EditorLauncher();
	~EditorLauncher() override;

	void Init(HINSTANCE hInstance, WNDPROC wndProc) override;
	void Update(const float& dt);
	void Render();
	const Timer& GetTimer() const override;
private:
	bool BeginFrame() override;
	void EndFrame() override;
	Editor* myEditor;
	GameApplication* myGameApplication;
};
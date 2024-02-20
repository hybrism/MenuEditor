#pragma once
#include <application/Application.h>

#include "MenuEditor.h"

class Engine;

class MenuEditorLauncher : public Application
{
public:
	MenuEditorLauncher();
	~MenuEditorLauncher() override;

	void Init(HINSTANCE hInstance, WNDPROC wndProc) override;
	void Update(const float& dt);
	void Render();

private:
	bool BeginFrame() override;
	void EndFrame() override;

	Engine* myEngine = nullptr;
	ME::MenuEditor myMenuEditor;
};

namespace Utility
{
	bool IsFbx(std::wstring aPath);
	bool IsDds(std::wstring aPath);
}
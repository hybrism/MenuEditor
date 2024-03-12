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

	MENU::MenuEditor myMenuEditor;
};

namespace MENU
{
	bool IsFbx(const std::wstring& aPath);
	bool IsDds(const std::wstring& aPath);
	bool IsTtf(const std::wstring& aPath);
	std::string ExtractFileName(const std::wstring& aPath);
}
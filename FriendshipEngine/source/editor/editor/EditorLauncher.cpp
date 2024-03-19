#include "EditorLauncher.h"

#include <application/ApplicationEntryPoint.cpp>
#include <game/application/GameApplication.h>

#include <engine/Engine.h>
#include <engine/graphics/GraphicsEngine.h>

//External
#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>

//Internal
#include "Editor.h"

#pragma comment(lib, "user32")
#pragma warning(disable : 4996)

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Application* CreateApplication() {
	return new EditorLauncher();
}

LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	LRESULT imguiResult = ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	if (imguiResult != 0)
	{
		return imguiResult;
	}

	return GameApplication::WindowProcedure(hWnd, uMsg, wParam, lParam);
}

EditorLauncher::EditorLauncher() : Application()
{
	myGameApplication = new GameApplication();
	myEditor = new Editor();
}

EditorLauncher::~EditorLauncher()
{
	delete myEditor;
	delete myGameApplication;
}

bool EditorLauncher::BeginFrame()
{
	return myGameApplication->BeginFrame();
}

void EditorLauncher::Init(HINSTANCE hInstance, WNDPROC wndProc)
{
	myGameApplication->Init(hInstance, wndProc);
	myEditor->Init(myGameApplication->GetGame());
}

void EditorLauncher::Update(const float& dt)
{
	myEditor->Update(dt);

	// TODO: Remove this, due to the current editor structure, it is updated before the game loop which in turn causes the draw calls to be updated after editor and reset at the start of the next frame
	// same todo can be found in GraphicsEngine.h
	GraphicsEngine::GetInstance()->ResetDrawCalls();

	myGameApplication->Update(dt);
}

void EditorLauncher::Render()
{
	myGameApplication->Render();
	GraphicsEngine::GetInstance()->SetRawBackBufferAsRenderTarget();
	myEditor->Render();
}

void EditorLauncher::EndFrame()
{
	myGameApplication->EndFrame();
}

const Timer& EditorLauncher::GetTimer() const
{
	return myGameApplication->GetTimer();
}

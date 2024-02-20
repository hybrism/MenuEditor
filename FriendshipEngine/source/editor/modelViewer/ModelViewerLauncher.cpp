#include "ModelViewerLauncher.h"

//Engine
#include <application/ApplicationEntryPoint.cpp>
#include <engine/Engine.h>
#include <engine/graphics/GraphicsEngine.h>
#include <engine/utility/Error.h>
#include <engine/utility/StringHelper.h>
#include <engine/graphics/Camera.h>
#include <engine/utility/InputManager.h>
#include <engine/Paths.h>

//External
#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>

#include <shared/imguiHandler/ImGuiHandler.h>
#include <shared/postMaster/PostMaster.h>

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Application* CreateApplication() {
	return new ModelViewerLauncher();
}

LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	LRESULT imguiResult = ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	if (imguiResult != 0)
	{
		return imguiResult;
	}

	if (IsWindowEnabled(hWnd))
	{
		if (InputManager::GetInstance()->UpdateEvents(uMsg, wParam, lParam))
			return 0;
	}

	switch (uMsg)
	{
	case WM_DROPFILES:
	{
		HDROP hDropInfo = (HDROP)wParam;

		UINT num_paths = DragQueryFileW(hDropInfo, 0xFFFFFFFF, 0, 512);

		wchar_t* filename = nullptr;
		UINT max_filename_len = 0;

		for (UINT i = 0; i < num_paths; ++i)
		{
			UINT filename_len = DragQueryFileW(hDropInfo, i, nullptr, 512) + 1;

			if (filename_len > max_filename_len)
			{
				max_filename_len = filename_len;
				wchar_t* tmp = (wchar_t*)realloc(filename, max_filename_len * sizeof(*filename));

				if (tmp != nullptr)
				{
					filename = tmp;
				}
			}

			DragQueryFileW(hDropInfo, i, filename, filename_len);
		}

		if (filename > 0 && Utility::IsFbx(filename))
		{
			Print("You dropped a FBX!");

			FE::PostMaster::GetInstance()->SendMessage({ FE::eMessageType::MeshDropped, StringHelper::ws2s(filename) });
		}

		free(filename);
		DragFinish(hDropInfo);
		break;
	}
	default:
		return ::Application::WindowProcedure(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

ModelViewerLauncher::ModelViewerLauncher()
{
	myEngine = Engine::CreateInstance(&myTimer);
}

ModelViewerLauncher::~ModelViewerLauncher()
{
	InputManager::GetInstance()->DestroyInstance();
	myEngine->DestroyInstance();
	myEngine = nullptr;
}

void ModelViewerLauncher::Init(HINSTANCE hInstance, WNDPROC wndProc)
{
	Vector2i resolution = { 1000, 900 };
	std::wstring icon = std::wstring(StringHelper::s2ws(RELATIVE_EDITOR_ASSET_PATH) + L"icon.ico");
	const wchar_t* ICON_PATH = icon.c_str();

	if (!myEngine->Init(resolution.x, resolution.y, L"Engine", L"FriendshipEngine", ICON_PATH, hInstance, wndProc))
	{
		PrintE("Engine could not Initiate!");
		return;
	}

	HWND windowHandle = myEngine->GetWindowHandle();
	DragAcceptFiles(windowHandle, TRUE);

	GraphicsEngine::GetInstance()->SetClearColor(0.f, 0.f, 0.f);

	ImGuiHandler::Init();

	myModelViewer.Init();
}


void ModelViewerLauncher::Update(const float& dt)
{
	::Application::Update(dt);
	ImGuiHandler::Update();

	InputManager::GetInstance()->Update();

	myModelViewer.Update(dt);

}

void ModelViewerLauncher::Render()
{
	myModelViewer.Render();

	GraphicsEngine::GetInstance()->SetRawBackBufferAsRenderTarget();
	ImGuiHandler::Render();
}

void ModelViewerLauncher::EndFrame()
{
	::Application::EndFrame();
	myEngine->EndFrame();
}


bool ModelViewerLauncher::BeginFrame()
{
	return myEngine->BeginFrame();
}

bool Utility::IsFbx(std::wstring aPath)
{
	return (aPath.substr(aPath.find_last_of(L".") + 1) == L"fbx");
}

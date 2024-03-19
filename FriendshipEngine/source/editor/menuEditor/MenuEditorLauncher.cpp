#include "MenuEditorLauncher.h"

//Engine
#include <application/ApplicationEntryPoint.cpp>

#include <engine/Engine.h>
#include <engine/utility/Error.h>
#include <engine/Paths.h>
#include <engine/utility/StringHelper.h>
#include <engine/utility/InputManager.h>

//External
#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>

#include <shared/imguiHandler/ImGuiHandler.h>
#include <shared/postMaster/PostMaster.h>

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Application* CreateApplication() {
	return new MenuEditorLauncher();
}

LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	LRESULT imguiResult = ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	if (imguiResult != 0)
	{
		ImGuiIO& io = ImGui::GetIO();
	
		if (io.WantCaptureMouse && (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP || uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP || uMsg == WM_MOUSEWHEEL || uMsg == WM_MOUSEMOVE))
			return true;

		//return imguiResult;
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

		wchar_t* filePath = nullptr;
		UINT max_filePath_len = 0;

		for (UINT i = 0; i < num_paths; ++i)
		{
			UINT filename_len = DragQueryFileW(hDropInfo, i, nullptr, 512) + 1;

			if (filename_len > max_filePath_len)
			{
				max_filePath_len = filename_len;
				wchar_t* tmp = (wchar_t*)realloc(filePath, max_filePath_len * sizeof(*filePath));

				if (tmp != nullptr)
				{
					filePath = tmp;
				}
			}

			DragQueryFileW(hDropInfo, i, filePath, filename_len);
		}

		if (filePath > 0 && MENU::IsFbx(filePath))
		{
			FE::PostMaster::GetInstance()->SendMessage({ FE::eMessageType::MeshDropped, StringHelper::ws2s(filePath) });
		}

		if (filePath > 0 && MENU::IsDds(filePath))
		{
			std::string filenameOnly = MENU::ExtractFileName(filePath);
			std::wstring copyTo = StringHelper::s2ws(RELATIVE_SPRITE_ASSET_PATH + filenameOnly);

			CopyFile(filePath, copyTo.c_str(), true);

			FE::PostMaster::GetInstance()->SendMessage({ FE::eMessageType::DdsDropped, filenameOnly });
		}

		if (filePath > 0 && MENU::IsTtf(filePath))
		{
			std::string filenameOnly = MENU::ExtractFileName(filePath);
			std::wstring copyTo = StringHelper::s2ws(RELATIVE_FONT_ASSET_PATH + filenameOnly);

			CopyFile(filePath, copyTo.c_str(), true);

			FE::PostMaster::GetInstance()->SendMessage({ FE::eMessageType::TtfDropped, filenameOnly });
		}

		free(filePath);
		DragFinish(hDropInfo);
		break;
	}
	default:
		return ::Application::WindowProcedure(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

MenuEditorLauncher::MenuEditorLauncher()
	: Application()
{
	myEngine = Engine::CreateInstance(&myTimer);
}

MenuEditorLauncher::~MenuEditorLauncher()
{
	InputManager::GetInstance()->DestroyInstance();
	myEngine->DestroyInstance();
	myEngine = nullptr;
}

void MenuEditorLauncher::Init(HINSTANCE hInstance, WNDPROC wndProc)
{
	Vector2i resolution = { 1600, 900 };
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

	myMenuEditor.Init();
}


void MenuEditorLauncher::Update(const float& dt)
{
	::Application::Update(dt);
	ImGuiHandler::Update();

	InputManager::GetInstance()->Update();

	myMenuEditor.Update(dt);

}

void MenuEditorLauncher::Render()
{
	myMenuEditor.Render();

	GraphicsEngine::GetInstance()->SetRawBackBufferAsRenderTarget();
	ImGuiHandler::Render();
}

void MenuEditorLauncher::EndFrame()
{
	::Application::EndFrame();
	myEngine->EndFrame();
}


bool MenuEditorLauncher::BeginFrame()
{
	return myEngine->BeginFrame();
}

bool MENU::IsFbx(const std::wstring& aPath)
{
	return (aPath.substr(aPath.find_last_of(L".") + 1) == L"fbx");
}

bool MENU::IsDds(const std::wstring& aPath)
{
	return (aPath.substr(aPath.find_last_of(L".") + 1) == L"dds");
}

bool MENU::IsTtf(const std::wstring& aPath)
{
	return (aPath.substr(aPath.find_last_of(L".") + 1) == L"ttf");
}

std::string MENU::ExtractFileName(const std::wstring& aPath)
{
	return StringHelper::ws2s(aPath).substr(StringHelper::ws2s(aPath).find_last_of("\\") + 1);
}

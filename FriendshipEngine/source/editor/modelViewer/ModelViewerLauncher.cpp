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


#include <engine/graphics/renderer/DeferredRenderer.h>
#include <engine/graphics/renderer/ForwardRenderer.h>
#include <assets/ShaderDatabase.h>
#include <engine/shaders/VertexShader.h>
#include <engine/shaders/PixelShader.h>

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

	if (!myEngine->Init(resolution.x, resolution.y, L"Engine", L"FriendshipEngine", L"icon.ico", hInstance, wndProc))
	{
		PrintE("Engine could not Initiate!");
		return;
	}

	HWND windowHandle = myEngine->GetWindowHandle();
	DragAcceptFiles(windowHandle, TRUE);

	GraphicsEngine::GetInstance()->SetClearColor(0.f, 0.f, 0.f);

	ImGuiHandler::Init();

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Button] = ImVec4(0.37f, 0.38f, 0.38f, 0.54f);

	myPostProcess.Init();

	myLightManager.Init({ 0.f,-1.f,0.f }, { 118.f / 256.f, 75.f / 256.f, 35.f / 256.f ,0 }, { 0, 10, 0 });
	myLightManager.SetAmbientLight(1.0f);

	myModelViewer.Init(&myLightManager);
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

	GraphicsEngine* ge = GraphicsEngine::GetInstance();
	GBuffer& gBuffer = ge->GetGBuffer();
	DeferredRenderer& deferred = ge->GetDeferredRenderer();
	ForwardRenderer& forward = ge->GetForwardRenderer();
	auto prevDrawCalls = ge->GetDrawCalls();

	// Shadows
	myLightManager.BeginShadowRendering();
	deferred.DoShadowRenderPass();
	forward.DoShadowRenderPass();
	myLightManager.EndShadowRendering();

	if ((ge->GetDrawCalls() - prevDrawCalls) > 0)
	{
		deferred.DoGBufferPass();
		//mySkyBox.Render(); //TO:DO Find a nicer place for the SkyBox to live.  Utkommenterad tills jag vet varför vingarna försvinner när man titttar på skyboxen

		myPostProcess.FirstFrame();
		{
			gBuffer.SetAllAsResources(1);

			myLightManager.LightRender();
			myLightManager.SetResourcesOnSlot();

			deferred.DoFinalPass();
			forward.DoRenderPass();
		}

		myLightManager.ClearAllResources();
	}
	myPostProcess.Render();

	{
		ge->SetRawBackBufferAsRenderTarget();

		auto* context = ge->DX().GetContext();

		context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
		context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(nullptr);

		context->GSSetShader(nullptr, nullptr, 0);

		ShaderDatabase::GetVertexShader(VsType::Fullscreen)->PrepareRender();
		ShaderDatabase::GetPixelShader(PsType::Fullscreen)->PrepareRender();

		ge->SetRawBackBufferAsRenderTarget();

		context->PSSetShaderResources(0, 1, ge->GetBackBufferSRV().GetAddressOf());
		context->Draw(3, 0);

		ID3D11ShaderResourceView* nullSRV = nullptr;
		context->PSSetShaderResources(0, 1, &nullSRV);
	}

	myModelViewer.RenderImGui();

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

#include "Launcher.h"

#include <game/application/GameApplication.h>
#include <application/ApplicationEntryPoint.cpp>

#include <engine/graphics/GraphicsEngine.h>
#include <assets/ShaderDatabase.h>
#include <engine/shaders/VertexShader.h>
#include <engine/shaders/PixelShader.h>

Application* CreateApplication() {
	return new Launcher();
}

LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	return GameApplication::WindowProcedure(hWnd, uMsg, wParam, lParam);
}

Launcher::Launcher() : Application()
{
	myGameApplication = new GameApplication();
}

Launcher::~Launcher()
{
	delete myGameApplication;
}

bool Launcher::BeginFrame()
{
	return myGameApplication->BeginFrame();
}

void Launcher::Init(HINSTANCE hInstance, WNDPROC wndProc)
{
	myGameApplication->Init(hInstance, wndProc);
}

void Launcher::Update(const float& dt,const double& aTotalTime)
{
	myGameApplication->Update(dt, aTotalTime);
}

void Launcher::Render()
{
	myGameApplication->Render();
	RenderBackbuffer();
}

void Launcher::RenderBackbuffer()
{
	auto* ge = GraphicsEngine::GetInstance();
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

#ifndef _RELEASE
	ge->IncrementDrawCalls();
#endif
}

const Timer& Launcher::GetTimer() const
{
	return myGameApplication->GetTimer();
}
void Launcher::EndFrame()
{
	myGameApplication->EndFrame();
}

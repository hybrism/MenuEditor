#include "pch.h"
#include "PostProcess.h"

#include "Vertex.h"
#include "GraphicsData.h"
#include "../graphics/RenderTarget.h"
#include "../shaders/VertexShader.h"
#include "../shaders/PixelShader.h"
#include "../utility/Error.h"
#include "GraphicsEngine.h"
#include "../Paths.h"
#include <assets/ShaderDatabase.h>

PostProcess::PostProcess() = default;

PostProcess::~PostProcess()
{
	myDownScaledRenderTargets.clear();
}

void PostProcess::Init()
{
	auto* ge = GraphicsEngine::GetInstance();

	Vector2i size = ge->GetViewportDimensions();
	myRenderTargets[0] = RenderTarget::Create(size, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
	myRenderTargets[1] = RenderTarget::Create(size, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);

	static int divider = 2;
	// Create several rendertargets with different viewtargets for blur.
	for (int i = 1; i < 5; i++)
	{
		size = ge->GetViewportDimensions();
		size.x /= divider;
		size.y /= divider;

		RenderTargetWrapper wrapper;
		wrapper.downscaleLevel = divider;
		wrapper.target = RenderTarget::Create(size, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
		myDownScaledRenderTargets.push_back(wrapper);

		divider = divider * 2;
	}
}

#if !USE_POST_PROCESSING
#pragma warning( push )
#pragma warning( disable : 4702)
#endif

void PostProcess::FirstFrame()
{
	auto ge = GraphicsEngine::GetInstance();

#if !USE_POST_PROCESSING
	ge->SetBackBufferAsActiveTarget();
	return;
#endif

#ifdef _DEBUG
	if (ge->GetRenderMode() != 0)
	{
		ge->SetBackBufferAsActiveTarget();
		return;
	}
#endif

	// TODO: Should fetch clear color from engine
	ClearShaderResources();

	myRenderTargets[0].SetAsTarget(&ge->GetDepthBuffer());
	auto clearColor = ge->GetClearColor();
	myRenderTargets[0].Clear(clearColor);
	myRenderTargets[1].Clear(clearColor);
}

void PostProcess::Render()
{
	auto ge = GraphicsEngine::GetInstance();

#if !USE_POST_PROCESSING
	return;
#endif

#ifdef _DEBUG
	if (ge->GetRenderMode() != 0)
	{
		return;
	}
#endif

	auto context = ge->GetContext();
	ShaderDatabase::GetVertexShader(VsType::Fullscreen)->PrepareRender();
	ge->SetDepthStencilState(DepthStencilState::Disabled);
	{
		ge->SetBlendState(BlendState::Disabled);

		myBufferData.downScaleLevel = myDownScaledRenderTargets[0].downscaleLevel;
		ShaderDatabase::GetPixelShader(PsType::PostProcess)->PrepareRender(BufferSlots::PostProcess, &myBufferData, sizeof(myBufferData));
		{
			myDownScaledRenderTargets[0].target.SetAsTarget(nullptr);
			context->PSSetShaderResources(11, 1, myRenderTargets[0].SRV.GetAddressOf());
		}
		context->Draw(3, 0);

		// Down scale
		for (int i = 1; i < myDownScaledRenderTargets.size(); i++)
		{
			myDownScaledRenderTargets[i].target.SetAsTarget(nullptr);
			context->PSSetShaderResources(11, 1, myDownScaledRenderTargets[i - 1].target.SRV.GetAddressOf());

			myBufferData.downScaleLevel = myDownScaledRenderTargets[i].downscaleLevel;
			ShaderDatabase::GetPixelShader(PsType::PostProcess)->PrepareRender(BufferSlots::PostProcess, &myBufferData, sizeof(myBufferData));

			context->Draw(3, 0);
		}

		ge->SetBlendState(BlendState::AlphaBlend);

		// Up scale
		for (int i = (int)myDownScaledRenderTargets.size() - 1; i >= 1; i--)
		{
			myDownScaledRenderTargets[i - 1].target.SetAsTarget(nullptr);
			context->PSSetShaderResources(11, 1, myDownScaledRenderTargets[i].target.SRV.GetAddressOf());

			myBufferData.downScaleLevel = myDownScaledRenderTargets[i].downscaleLevel;
			ShaderDatabase::GetPixelShader(PsType::PostProcess)->PrepareRender(BufferSlots::PostProcess, &myBufferData, sizeof(myBufferData));

			context->Draw(3, 0);
		}


		myBufferData.downScaleLevel = 0;
		ShaderDatabase::GetPixelShader(PsType::PostProcess)->PrepareRender(BufferSlots::PostProcess, &myBufferData, sizeof(myBufferData));
		{
			myRenderTargets[0].SetAsTarget(nullptr);
			context->PSSetShaderResources(11, 1, myDownScaledRenderTargets[0].target.SRV.GetAddressOf());
		}
		context->Draw(3, 0);
	}

	ge->SetBlendState(BlendState::Disabled);
	ShaderDatabase::GetPixelShader(PsType::Vignette)->PrepareRender();
	{
		myRenderTargets[1].SetAsTarget(nullptr);

		context->PSSetShaderResources(11, 1, myRenderTargets[0].SRV.GetAddressOf());
	}
	context->Draw(3, 0);


	ShaderDatabase::GetPixelShader(PsType::ToneMap)->PrepareRender();
	{
		ge->SetBlendState(BlendState::AlphaBlend);
		ge->SetBackBufferAsActiveTarget();
		ge->GetBackBufferRenderTarget().Clear(ge->GetClearColor());
		context->PSSetShaderResources(11, 1, myRenderTargets[1].SRV.GetAddressOf());
	}
	context->Draw(3, 0);

	ge->SetBlendState(BlendState::Disabled);
	ge->SetDepthStencilState(DepthStencilState::ReadWrite);
	ClearShaderResources();
}


#if !USE_POST_PROCESSING
#pragma warning( pop ) 
#endif

void PostProcess::ClearShaderResources()
{
	ID3D11ShaderResourceView* nullSRV = nullptr;
	GraphicsEngine::GetInstance()->GetContext()->PSSetShaderResources(11, 1, &nullSRV);
}

#include "pch.h"
#include "DxData.h"
#include <utility/Error.h>

DxData::DxData()
{
}

DxData::~DxData()
{
}

bool DxData::Init(int aWidth, int aHeight, HWND aWND)
{
	HRESULT result;
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	//swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	//swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	//swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = aWND;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = 0;
	UINT creationFlags = 0;
#if defined(REPORT_DX_WARNINGS)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creationFlags, nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc, &mySwapChain, &myDevice, nullptr, &myContext);

	ID3D11Texture2D* backBufferTexture;
	result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);

	if (FAILED(result))
	{
		PrintE("Could not get BackBuffer texture");
		return false;
	}
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	result = myDevice->CreateRenderTargetView(backBufferTexture, &rtvDesc, &myBackBuffer);

	if (FAILED(result))
	{
		PrintE("Could not create RenderTargetView");
		return false;
	}

#if defined(REPORT_DX_WARNINGS)
	{
		ComPtr<ID3D11Debug> d3dDebug;
		result = myDevice.As(&d3dDebug);
		if (FAILED(result))
		{
			PrintE("Could not get Debug interface");
			return false;
		}
		ComPtr<ID3D11InfoQueue> d3dInfoQueue;
		result = d3dDebug.As(&d3dInfoQueue);
		if (FAILED(result))
		{
			PrintE("Could not report live device objects");
			return false;
		}

		result = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
		if (FAILED(result))
		{
			PrintE("Could not report live device objects");
			return false;
		}


#ifndef _RELEASE
		d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
		d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
		D3D11_MESSAGE_ID hide[] =
		{
			D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
			// TODO: Add more message IDs here as needed
		};
		D3D11_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = _countof(hide);
		filter.DenyList.pIDList = hide;
		d3dInfoQueue->AddStorageFilterEntries(&filter);
	}
#endif


	D3D11_TEXTURE2D_DESC textureDesc;
	backBufferTexture->GetDesc(&textureDesc);
	backBufferTexture->Release();

	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), nullptr);

	myViewport = { 0 };
	myViewport.TopLeftX = 0.0f;
	myViewport.TopLeftY = 0.0f;
	myViewport.Width = static_cast<float>(textureDesc.Width);
	myViewport.Height = static_cast<float>(textureDesc.Height);
	myViewport.MinDepth = 0.0f;
	myViewport.MaxDepth = 1.0f;
	myContext->RSSetViewports(1, &myViewport);

	myWindowDimensions = { aWidth, aHeight };
	myViewportDimensions = { (int)textureDesc.Width, (int)textureDesc.Height };

	myDepthBuffer = DepthBuffer::Create(myViewportDimensions);

	return true;
}

void DxData::EndFrame()
{
	mySwapChain->Present(1, (UINT)mySwapChainState);
	myDepthBuffer.CopyToStaging();
}

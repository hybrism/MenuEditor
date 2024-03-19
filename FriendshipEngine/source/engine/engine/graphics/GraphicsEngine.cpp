#include "pch.h"
#define STB_IMAGE_IMPLEMENTATION
#include "GraphicsEngine.h"
#include "../graphics/Camera.h"
#include "../text/TextService.h"
#include "../utility/Error.h"
#include <application/Timer.h>
#include "../math/helper.h"
#include <iostream>
#include <directxtk/DDSTextureLoader.h>
#include "../Paths.h"
#include <assets/ModelFactory.h>
#include "../Engine.h"
#include "Vertex.h"
#include "GraphicsData.h"
#include "GBuffer.h"
//#include "DirectionalLightManager.h"

#include "sprite/SpriteDrawer.h"
#include "model/MeshDrawer.h"
#include "../utility/StringHelper.h"
#include "../../assets/ShaderDatabase.h"
#include "RenderTarget.h"

#define REPORT_DX_WARNINGS

GraphicsEngine* GraphicsEngine::myInstance = nullptr;

ComPtr<ID3D11RenderTargetView>& GraphicsEngine::GetBackBuffer() { return myBackBufferRenderTarget.RTV; }
ComPtr<ID3D11ShaderResourceView>& GraphicsEngine::GetBackBufferSRV() { return myBackBufferRenderTarget.SRV; }

RenderTarget& GraphicsEngine::GetBackBufferRenderTarget()
{
	return myBackBufferRenderTarget;
}

GraphicsEngine::GraphicsEngine() : myRenderers()
{

#ifndef _RELEASE
	SetClearColor({ 64.0f / 256.0f, 166.0f / 256.0f, 245.0f / 256.0f });
	//SetClearColor(0.14f, 0.38f, 0.8f);
#else
	SetClearColor({ 0.93f, 1.f, 0.83f });
#endif // _DEBUG
}

GraphicsEngine::~GraphicsEngine()
{
	delete myViewCamera;

	ShaderDatabase::DestroyInstance(); // move responsibility else where?
}

bool GraphicsEngine::Initialize(int aWidth, int aHeight, HWND aHWND)
{
	HRESULT result;

	myDxData.Init(aWidth, aHeight, aHWND);

	// TODO: Dont crash the entire engine when cube map is missing
	SetCubemap(std::string(RELATIVE_CUBEMAP_ASSET_PATH) + "CubeMap_Skansen 1.dds");

	myViewCamera = new Camera();

	auto& device = myDxData.GetDevice();
	auto windowDimensions = myDxData.GetWindowDimensions();
	auto viewportDimensions = myDxData.GetViewportDimensions();
	myViewCamera->SetProjectionMatrix(100.0f, (float)windowDimensions.x, (float)windowDimensions.y, 1.1f, 100000.f);
	{
		D3D11_BUFFER_DESC bufferDescription = { 0 };
		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
		bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDescription.ByteWidth = sizeof(FrameBufferData);
		result = device->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer);
		if (FAILED(result))
		{
			PrintE("Could not create FrameBuffer");
			return false;
		}

		bufferDescription.ByteWidth = sizeof(ObjectBufferData);
		result = device->CreateBuffer(&bufferDescription, nullptr, &myObjectBuffer);
		if (FAILED(result))
		{
			PrintE("Could not create ObjectBuffer");
			return false;
		}
	}

	myGBuffer = GBuffer::Create(viewportDimensions);

	{
		D3D11_BUFFER_DESC bufferDescription = { 0 };
		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
		bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDescription.ByteWidth = sizeof(InputBufferData);
		result = device->CreateBuffer(&bufferDescription, nullptr, &myInputBuffer);

		if (FAILED(result))
		{
			PrintE("Could not create InputBuffer");
			return false;
		}
	}

	ShaderDatabase::CreateInstance();

	myRenderers.meshDrawer.Init();
	myRenderers.debugRenderer.Init();
	myRenderers.spriteRenderer.Init();

	//myDirectionalLightManager = new DirectionalLightManager();
	//myDirectionalLightManager->Init();

	myCurrentCamera = myViewCamera;

	if (!CreateBlendStates())
		return false;
	if (!CreateDepthStencilStates())
		return false;
	if (!CreateRasterizerStates())
		return false;
	if (!CreateSamplerStates())
		return false;

	myBackBufferRenderTarget = RenderTarget::Create(viewportDimensions, DXGI_FORMAT_R8G8B8A8_UNORM);

	// TODO: Use relative asset path
	// TODO: Dont crash the entire engine when cube map is missing
	SetCubemap(std::string(RELATIVE_CUBEMAP_ASSET_PATH) + "CubeMap_Skansen 1.dds");

	return true;
}

void GraphicsEngine::ChangeCurrentCamera(Camera* aCamera)
{
	if (aCamera == myCurrentCamera) { return; }
	myCurrentCamera = aCamera;
	UpdateFrameBuffer();
}

void GraphicsEngine::ResetToViewCamera()
{
	ChangeCurrentCamera(myViewCamera);
}

void GraphicsEngine::SetUseOfFreeCamera(const bool& aBool)
{
	myIsUsingViewCamera = aBool;
}

void GraphicsEngine::SetRenderState(const RenderState& aRenderState)
{
	myRenderState = aRenderState;

	// We dont really need to all blendstates when this function is called
	// but it will do for now

	SetRasterizerState(aRenderState.rasterizerState);
	SetDepthStencilState(aRenderState.depthStencilState);
	SetBlendState(aRenderState.blendState);
	SetSamplerState(aRenderState.samplerFilter, aRenderState.samplerAddressMode);
}

void GraphicsEngine::SetSamplerState(const SamplerFilter& aSamplerFilter, const SamplerAddressMode& aSamplerAddressMode)
{
	auto* context = myDxData.GetContext();
	context->PSSetSamplers(0, 1, myRenderStates.mySamplerStates[(int)aSamplerFilter][(int)aSamplerAddressMode].GetAddressOf());
	context->VSSetSamplers(0, 1, myRenderStates.mySamplerStates[(int)aSamplerFilter][(int)aSamplerAddressMode].GetAddressOf());
}

void GraphicsEngine::SetDepthStencilState(const DepthStencilState& aDepthStencilState)
{
	myDxData.GetContext()->OMSetDepthStencilState(myRenderStates.myDepthStencilStates[(int)aDepthStencilState].Get(), 0xffffffff);
}

void GraphicsEngine::SetCubemap(const std::string& someCubemapLocation)
{
	HRESULT result = DirectX::CreateDDSTextureFromFile(myDxData.GetDevice().Get(), StringHelper::s2ws(someCubemapLocation).c_str(), nullptr, &myCubemap);
	assert(!FAILED(result));

	ID3D11Resource* theResource;
	myCubemap->GetResource(&theResource);
	ID3D11Texture2D* cubeTexture = reinterpret_cast<ID3D11Texture2D*>(theResource);
	D3D11_TEXTURE2D_DESC cubeDescription;
	cubeTexture->GetDesc(&cubeDescription);

	myNumMips = cubeDescription.MipLevels;
}

void GraphicsEngine::UpdateFrameBuffer()
{
	auto windowDimensions = myDxData.GetWindowDimensions();
	auto viewportDimensions = myDxData.GetViewportDimensions();
	auto* context = myDxData.GetContext();

	// Prepare FrameBufferData
	// This sets up the camera on the GPU
	FrameBufferData frameBufferData = {};
	frameBufferData.resolution = { (float)windowDimensions.x, (float)windowDimensions.y, (float)viewportDimensions.x, (float)viewportDimensions.y };
	frameBufferData.worldToClipMatrix = myCurrentCamera->GetProjectionMatrix();
	frameBufferData.modelToWorld = myCurrentCamera->GetModelMatrix();
	frameBufferData.nearPlane = myCurrentCamera->GetNearPlane();
	frameBufferData.farPlane = myCurrentCamera->GetFarPlane();

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	context->Map(myFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

	memcpy(mappedBuffer.pData, &frameBufferData, sizeof(FrameBufferData));

	context->Unmap(myFrameBuffer.Get(), 0);

	context->VSSetConstantBuffers((UINT)BufferSlots::Frame, 1, myFrameBuffer.GetAddressOf());
	context->PSSetConstantBuffers((UINT)BufferSlots::Frame, 1, myFrameBuffer.GetAddressOf());
}

void GraphicsEngine::SetBlendState(const BlendState& aBlendState)
{
	myDxData.GetContext()->OMSetBlendState(myRenderStates.myBlendStates[(int)aBlendState].Get(), nullptr, 0xffffffffu);
}

float GraphicsEngine::GetHeight(float x, float y)
{
	return 3 * (y * sin(0.1f * x) + x * cos(0.1f * y));
}

bool GraphicsEngine::CreateDepthStencilStates()
{
	HRESULT result = S_OK;
	D3D11_DEPTH_STENCIL_DESC desc = {};
	auto& device = myDxData.GetDevice();

	// Read only
	{
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		desc.StencilEnable = false;

		result = device->CreateDepthStencilState(&desc, &myRenderStates.myDepthStencilStates[(int)DepthStencilState::ReadOnly]);
		if (FAILED(result))
			return false;
	}

	// Read only greater
	{
		desc = {};
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_GREATER;
		desc.StencilEnable = false;
		result = device->CreateDepthStencilState(&desc, &myRenderStates.myDepthStencilStates[(int)DepthStencilState::ReadOnlyGreater]);
		if (FAILED(result))
			return false;
	}

	// Disabled
	{
		desc = {};
		desc.DepthEnable = false;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		desc.StencilEnable = false;
		result = device->CreateDepthStencilState(&desc, &myRenderStates.myDepthStencilStates[(int)DepthStencilState::Disabled]);
		if (FAILED(result))
			return false;
	}

	// Read write
	{
		myRenderStates.myDepthStencilStates[(int)DepthStencilState::ReadWrite] = nullptr;
	}

	// Addative??
	{
		ZeroMemory(&desc, sizeof(desc));

		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		desc.StencilEnable = false;  //Fuckar
		desc.StencilReadMask = 0xff;
		desc.StencilWriteMask = 0xff;
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP; //desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		result = device->CreateDepthStencilState(&desc, &myRenderStates.myDepthStencilStates[(int)DepthStencilState::Additive]);
		if (FAILED(result))
			return false;
	}

	return true;
}

bool GraphicsEngine::CreateRasterizerStates()
{
	HRESULT result = S_OK;

	auto& device = myDxData.GetDevice();

	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = D3D11_FILL_WIREFRAME;
	desc.CullMode = D3D11_CULL_BACK;
	desc.DepthClipEnable = true;

	result = device->CreateRasterizerState(&desc, &myRenderStates.myRasterizerStates[(int)RasterizerState::Wireframe]);
	if (FAILED(result))
		return false;

	desc.CullMode = D3D11_CULL_NONE;

	result = device->CreateRasterizerState(&desc, &myRenderStates.myRasterizerStates[(int)RasterizerState::WireframeNoCulling]);
	if (FAILED(result))
		return false;

	desc = {};
	desc.AntialiasedLineEnable = false;
	desc.CullMode = D3D11_CULL_NONE;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.DepthClipEnable = true;
	desc.FillMode = D3D11_FILL_SOLID;
	desc.FrontCounterClockwise = false;
	desc.MultisampleEnable = true;
	desc.ScissorEnable = false;
	desc.SlopeScaledDepthBias = 0.0f;

	result = device->CreateRasterizerState(&desc, &myRenderStates.myRasterizerStates[(int)RasterizerState::NoFaceCulling]);
	if (FAILED(result))
		return false;


	desc = {};
	desc.AntialiasedLineEnable = false;
	desc.CullMode = D3D11_CULL_FRONT;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.DepthClipEnable = true;
	desc.FillMode = D3D11_FILL_SOLID;
	desc.FrontCounterClockwise = false;
	desc.MultisampleEnable = true;
	desc.ScissorEnable = false;
	desc.SlopeScaledDepthBias = 0.0f;

	result = device->CreateRasterizerState(&desc, &myRenderStates.myRasterizerStates[(int)RasterizerState::FrontfaceCulling]);
	if (FAILED(result))
		return false;

	myRenderStates.myRasterizerStates[(int)RasterizerState::BackfaceCulling] = nullptr;

	return true;
}

bool GraphicsEngine::CreateSamplerStates()
{
	auto& device = myDxData.GetDevice();

	HRESULT result = S_OK;
	D3D11_SAMPLER_DESC samplerDesc = {};

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = device->CreateSamplerState(&samplerDesc, myRenderStates.mySamplerStates[(int)SamplerFilter::Point][(int)SamplerAddressMode::Clamp].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = device->CreateSamplerState(&samplerDesc, myRenderStates.mySamplerStates[(int)SamplerFilter::Point][(int)SamplerAddressMode::Wrap].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = device->CreateSamplerState(&samplerDesc, myRenderStates.mySamplerStates[(int)SamplerFilter::Point][(int)SamplerAddressMode::Mirror].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = device->CreateSamplerState(&samplerDesc, myRenderStates.mySamplerStates[(int)SamplerFilter::Bilinear][(int)SamplerAddressMode::Clamp].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = device->CreateSamplerState(&samplerDesc, myRenderStates.mySamplerStates[(int)SamplerFilter::Bilinear][(int)SamplerAddressMode::Wrap].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = device->CreateSamplerState(&samplerDesc, myRenderStates.mySamplerStates[(int)SamplerFilter::Bilinear][(int)SamplerAddressMode::Mirror].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = device->CreateSamplerState(&samplerDesc, myRenderStates.mySamplerStates[(int)SamplerFilter::Trilinear][(int)SamplerAddressMode::Clamp].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = device->CreateSamplerState(&samplerDesc, myRenderStates.mySamplerStates[(int)SamplerFilter::Trilinear][(int)SamplerAddressMode::Wrap].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = device->CreateSamplerState(&samplerDesc, myRenderStates.mySamplerStates[(int)SamplerFilter::Trilinear][(int)SamplerAddressMode::Mirror].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void GraphicsEngine::SetRasterizerState(const RasterizerState& aRasterizerState)
{
	myDxData.GetContext()->RSSetState(myRenderStates.myRasterizerStates[(int)aRasterizerState].Get());
}

void GraphicsEngine::BeginFrame()
{
	//#ifndef _RELEASE
		//myDrawCalls = 0;
	//#endif
	UpdateFrameBuffer();
	
	auto* context = myDxData.GetContext();
	{
		// Input Data
		InputBufferData inputData = {};
		inputData.time = (float)Engine::GetInstance()->GetTimer()->GetTotalTime();
		inputData.deltaTime = (float)Engine::GetInstance()->GetTimer()->GetDeltaTime();
		inputData.renderMode = myRenderMode;

		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		context->Map(myInputBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &inputData, sizeof(InputBufferData));
		context->Unmap(myInputBuffer.Get(), 0);

		context->VSSetConstantBuffers((UINT)BufferSlots::Input, 1, myInputBuffer.GetAddressOf());
		context->PSSetConstantBuffers((UINT)BufferSlots::Input, 1, myInputBuffer.GetAddressOf());
	}
	{
		//set sampler
		context->PSSetSamplers(0, 1, myRenderStates.mySamplerStates[(int)SamplerFilter::Trilinear][(int)SamplerAddressMode::Wrap].GetAddressOf());
		context->PSSetSamplers(1, 1, myRenderStates.mySamplerStates[(int)SamplerFilter::Trilinear][(int)SamplerAddressMode::Clamp].GetAddressOf());
		context->PSSetShaderResources(0, 1, myCubemap.GetAddressOf());
	}

	{
		context->OMSetRenderTargets(1, myBackBufferRenderTarget.RTV.GetAddressOf(), myDxData.GetDepthBuffer().GetDepthStencilView());
		{
			context->ClearRenderTargetView(myBackBufferRenderTarget.RTV.Get(), &myClearColor.x);

			Vector4f color = { 0.02f, 0.02f, 0.02f, 1 }; //DARK GREY
			context->ClearRenderTargetView(myDxData.GetBackBuffer().Get(), &color.x);
		}
		myDxData.GetDepthBuffer().Clear();
	}
}

void GraphicsEngine::SetRawBackBufferAsRenderTarget()
{
	myDxData.GetContext()->OMSetRenderTargets(1, myDxData.GetBackBuffer().GetAddressOf(), nullptr);
}

void GraphicsEngine::SetRawBackBufferAsRenderTarget(DepthBuffer* aDepth)
{
	myDxData.GetContext()->OMSetRenderTargets(1, myDxData.GetBackBuffer().GetAddressOf(), aDepth->GetDepthStencilView());
}

void GraphicsEngine::EndFrame()
{
	myDxData.EndFrame();
#ifndef _RELEASE
	myGBuffer.CopyToStaging();
#endif
}

bool GraphicsEngine::CreateBlendStates()
{
	{
		HRESULT result = S_OK;
		D3D11_BLEND_DESC blendStateDescription = {};

		//Additive
		blendStateDescription = {};
		blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
		blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		result = GraphicsEngine::GetInstance()->DX().GetDevice()->CreateBlendState(&blendStateDescription, myRenderStates.myBlendStates[(int)BlendState::AdditiveBlend].ReleaseAndGetAddressOf());
		if (FAILED(result))
		{
			return false;
		}


		//Disabled
		blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
		blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		result = GraphicsEngine::GetInstance()->DX().GetDevice()->CreateBlendState(&blendStateDescription, myRenderStates.myBlendStates[(int)BlendState::Disabled].ReleaseAndGetAddressOf());
		if (FAILED(result))
		{
			return false;
		}

		//Alpha
		blendStateDescription = {};
		blendStateDescription.AlphaToCoverageEnable = false;
		blendStateDescription.IndependentBlendEnable = false;
		blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
		blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		result = GraphicsEngine::GetInstance()->DX().GetDevice()->CreateBlendState(&blendStateDescription, myRenderStates.myBlendStates[(int)BlendState::AlphaBlend].ReleaseAndGetAddressOf());





		//D3D11_BLEND_DESC blendStateDescription = {};
		//blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
		//blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		//blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		//blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		//blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		//blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		//blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		//blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		//blendStateDescription.IndependentBlendEnable = false;
		//auto result = GraphicsEngine::GetInstance()->DX().GetDevice()->CreateBlendState(&blendStateDescription, blendState.ReleaseAndGetAddressOf());






		if (FAILED(result))
		{
			return false;
		}

		return true;
	}
}

bool GraphicsEngine::SetResolution(Vector2i aResolution)
{
	aResolution;
	//https://stackoverflow.com/questions/27301550/change-resolution-in-directx-11


	//https://learn.microsoft.com/sv-se/windows/win32/direct3darticles/dxgi-best-practices?redirectedfrom=MSDN
	//DXGI_MODE_DESC newRes = {};
	//newRes.Width = aResolution.x;
	//newRes.Height = aResolution.y;
	//mySwapChain->ResizeTarget(&newRes);
	//mySwapChain->ResizeBuffers(10, aResolution.x, aResolution.y, DXGI_FORMAT_UNKNOWN, 0);



	//myWindowDimensions = aResolution;
	//myViewportDimensions = aResolution;
	//myBackBufferTextureSize = { static_cast<float>(aResolution.x), static_cast<float>(aResolution.y) };


	return true;
}

void GraphicsEngine::SetAsActiveTarget(
	const int& aNumViews,
	ComPtr<ID3D11RenderTargetView>* aViews,
	DepthBuffer* aDepth,
	const D3D11_VIEWPORT* aViewport
)
{
	const D3D11_VIEWPORT* viewport = aViewport ? aViewport : &myDxData.GetViewport();

	void* depth = myDxData.GetDepthBuffer().GetDepthStencilView();
	if (aDepth)
	{
		depth = aDepth->GetDepthStencilView();
	}

	auto* context = myDxData.GetContext();
	context->OMSetRenderTargets(aNumViews, aViews[0].GetAddressOf(), (ID3D11DepthStencilView*)depth);
	context->RSSetViewports(1, viewport);
}

void GraphicsEngine::SetBackBufferAsActiveTarget(DepthBuffer* aDepth)
{
	void* depth = aDepth;
	if (aDepth)
	{
		depth = aDepth->GetDepthStencilView();
	}
	myDxData.GetContext()->OMSetRenderTargets(1, GetBackBuffer().GetAddressOf(), (ID3D11DepthStencilView*)depth);
}

void GraphicsEngine::SetBackBufferAsActiveTarget()
{
	SetBackBufferAsActiveTarget(&myDxData.GetDepthBuffer());
}

void GraphicsEngine::IncrementRenderMode()
{
	myRenderMode = ++myRenderMode % (static_cast<unsigned int>(GBufferTexture::Count) + 1);
}

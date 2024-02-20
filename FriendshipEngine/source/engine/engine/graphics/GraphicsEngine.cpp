#include "pch.h"
#define STB_IMAGE_IMPLEMENTATION
#include "GraphicsEngine.h"
#include "../graphics/Camera.h"
#include "../text/TextService.h"
#include "../utility/Error.h"
#include <application/Timer.h>
#include "../math/helper.h"
#include <iostream>
#include <assets/DDSTextureLoader11.h>
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

ComPtr<ID3D11RenderTargetView>& GraphicsEngine::GetBackBuffer() { return myBackBufferRenderTarget->renderTargetView; }
ComPtr<ID3D11ShaderResourceView>& GraphicsEngine::GetBackBufferSRV() { return myBackBufferRenderTarget->shaderResourceView; }

GraphicsEngine::GraphicsEngine() : myMeshDrawer(), myDeferredRenderer(myMeshDrawer), myForwardRenderer(myMeshDrawer)
{
#ifdef _DEBUG
	SetClearColor(0, 0, 0);
	//SetClearColor(0.14f, 0.38f, 0.8f);
#else
	SetClearColor(0.93f, 1.f, 0.83f);
#endif // _DEBUG

	myClearColor.z = 1.f;
	myTextService = nullptr;
	mySpriteDrawer = nullptr;
}

GraphicsEngine::~GraphicsEngine()
{
	delete mySpriteDrawer;
	delete myTextService;
	delete myViewport;

	delete myViewCamera;

	ShaderDatabase::DestroyInstance(); // move responsibility else where?
}

bool GraphicsEngine::Initialize(int width, int height, HWND windowHandle)
{
	HRESULT result;
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = windowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;
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

	result = myDevice->CreateRenderTargetView(backBufferTexture, nullptr, &myBackBuffer);

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


#ifdef _DEBUG
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


	// TODO: Use relative asset path
	// TODO: Dont crash the entire engine when cube map is missing
	SetCubemap(std::string(RELATIVE_CUBEMAP_ASSET_PATH) + "cubemap.dds");

	D3D11_TEXTURE2D_DESC textureDesc;
	backBufferTexture->GetDesc(&textureDesc);
	backBufferTexture->Release();
	myBackBufferTextureSize = { static_cast<float>(textureDesc.Width) , static_cast<float>(textureDesc.Height) };

	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), nullptr);
	//D3D11_VIEWPORT viewport = { 0 };  //BYTT TILL EN PEKARE SOM ?R MEDLEMSVARIABLE
	myViewport = new D3D11_VIEWPORT;
	myViewport->TopLeftX = 0.0f;
	myViewport->TopLeftY = 0.0f;
	myViewport->Width = static_cast<float>(textureDesc.Width);
	myViewport->Height = static_cast<float>(textureDesc.Height);
	myViewport->MinDepth = 0.0f;
	myViewport->MaxDepth = 1.0f;
	myContext->RSSetViewports(1, myViewport);

	myWindowDimensions = { width, height };
	myViewportDimensions = { (int)textureDesc.Width, (int)textureDesc.Height };

	myViewCamera = new Camera();
	myViewCamera->SetProjectionMatrix(100.0f, (float)myWindowDimensions.x, (float)myWindowDimensions.y, 1.0f, 100000.f);
	myViewCamera->SetPosition({ 0, 1, -2 });
	{
		D3D11_BUFFER_DESC bufferDescription = { 0 };
		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
		bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDescription.ByteWidth = sizeof(FrameBufferData);
		result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer);
		if (FAILED(result))
		{
			PrintE("Could not create FrameBuffer");
			return false;
		}

		bufferDescription.ByteWidth = sizeof(ObjectBufferData);
		result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myObjectBuffer);
		if (FAILED(result))
		{
			PrintE("Could not create ObjectBuffer");
			return false;
		}
	}

	//{
	//	D3D11_BUFFER_DESC bufferDescription = { 0 };
	//	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	//	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//	bufferDescription.ByteWidth = sizeof(PointLightBufferData);
	//	result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myPointLightBuffer);
	//	if (FAILED(result))
	//	{
	//		PrintE("Could not create PointLightbuffer");
	//		return false;
	//	}
	//}

	myDepthBuffer = DepthBuffer::Create(myViewportDimensions);
	myGBuffer = GBuffer::Create(myViewportDimensions);

	{
		D3D11_BUFFER_DESC bufferDescription = { 0 };
		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
		bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDescription.ByteWidth = sizeof(InputBufferData);
		result = myDevice->CreateBuffer(&bufferDescription, nullptr, &myInputBuffer);

		if (FAILED(result))
		{
			PrintE("Could not create InputBuffer");
			return false;
		}
	}

	{
		// Create a texture sampler state description.
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		// Create the texture sampler state.
		result = myDevice->CreateSamplerState(&samplerDesc, &mySamplerState);
		if (FAILED(result))
		{
			PrintE("Could not create default SamplerState");
			return false;
		}
	}
	{
		// Create a texture sampler state description.
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = 0;
		// Create the texture sampler state.
		result = myDevice->CreateSamplerState(&samplerDesc, &my2dSamplerState);
		if (FAILED(result))
		{
			PrintE("Could not create 2D SamplerState");
			return false;
		}
	}
	{
		// Create a texture sampler that clamps.
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		// Create the texture sampler state.
		result = myDevice->CreateSamplerState(&samplerDesc, &myClampSamplerState);
		if (FAILED(result))
		{
			PrintE("Could not create clamp SamplerState");
			return false;
		}
	}

	{
		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = textureDesc.Width;
		desc.Height = textureDesc.Height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		result = myDevice->CreateRasterizerState(&rasterizerDesc, &myFrontFaceCullingRasterizerState);
		if (FAILED(result))
		{
			PrintE("Could not create RasterizerState");
			return false;
		}
	}


	ShaderDatabase::CreateInstance();

	myTextService = new TextService();
	myTextService->Init();

	mySpriteDrawer = new SpriteDrawer();
	mySpriteDrawer->Init();

	myMeshDrawer.Init();

	//myDirectionalLightManager = new DirectionalLightManager();
	//myDirectionalLightManager->Init();

	myCurrentCamera = myViewCamera;

	if (!CreateBlendStates())
		return false;
	if (!CreateDepthStencilStates())
		return false;
	if (!CreateRasterizerStates())
		return false;

	myBackBufferRenderTarget = new RenderTarget();
	*myBackBufferRenderTarget = RenderTarget::Create(myViewportDimensions, DXGI_FORMAT_R16G16B16A16_FLOAT);

	return true;
}

void GraphicsEngine::ChangeCurrentCamera(Camera* aCamera)
{
	if (aCamera == myCurrentCamera) { return; }
	myCurrentCamera = aCamera;
	UpdateFrameBuffer();
}

void GraphicsEngine::ResetToPrimaryCamera()
{
	ChangeCurrentCamera(myViewCamera);
}

void GraphicsEngine::SetRenderState(const RenderState& aRenderState)
{
	myRenderState = aRenderState;

	// We dont really need to all blendstates when this function is called
	// but it will do for now

	SetRasterizerState(aRenderState.rasterizerState);
	SetDepthStencilState(aRenderState.depthStencilState);
	SetBlendState(aRenderState.blendState);
}

void GraphicsEngine::CalculateBinormalTangent(Vertex* vertices, const size_t& vertexCount)
{
	size_t i;
	for (i = 0; i + 2 < vertexCount; i += 3)
	{
		Vertex& v0 = vertices[i];
		Vertex& v1 = vertices[i + 1];
		Vertex& v2 = vertices[i + 2];

		// Calculate edge vectors
		Vector3f edge1 = { v1.position.x - v0.position.x, v1.position.y - v0.position.y, v1.position.z - v0.position.z };
		Vector3f edge2 = { v2.position.x - v0.position.x, v2.position.y - v0.position.y, v2.position.z - v0.position.z };

		// Calculate delta UVs
		Vector2f deltaUV1 = { v1.uv.x - v0.uv.x, v1.uv.y - v0.uv.y };
		Vector2f deltaUV2 = { v2.uv.x - v0.uv.x, v2.uv.y - v0.uv.y };

		Vector3f tangent;
		{
			tangent.x = deltaUV2.y * edge1.x - deltaUV1.y * edge2.x;
			tangent.y = deltaUV2.y * edge1.y - deltaUV1.y * edge2.y;
			tangent.z = deltaUV2.y * edge1.z - deltaUV1.y * edge2.z;
			tangent.Normalize();
		}

		Vector3f binormal;
		{
			binormal.x = deltaUV1.x * edge2.x - deltaUV2.x * edge1.x;
			binormal.y = deltaUV1.x * edge2.y - deltaUV2.x * edge1.y;
			binormal.z = deltaUV1.x * edge2.z - deltaUV2.x * edge1.z;
			binormal.Normalize();
		}

		// Assign tangent and binormal to vertices
		v0.tangent = tangent;
		v1.tangent = tangent;
		v2.tangent = tangent;

		v0.binormal = binormal;
		v1.binormal = binormal;
		v2.binormal = binormal;
	}


	// Handle the remaining vertices (less than 3)
	if (i < vertexCount)
	{
		Vertex& v0 = vertices[i];

		if (i + 1 < vertexCount)
		{
			// Two remaining vertices
			Vertex& v1 = vertices[i + 1];

			// Calculate tangent and binormal for the remaining vertices
			Vector3f edge = { v1.position.x - v0.position.x, v1.position.y - v0.position.y, v1.position.z - v0.position.z };
			Vector2f deltaUV = { v1.uv.x - v0.uv.x, v1.uv.y - v0.uv.y };

			Vector3f tangent;
			{
				tangent.x = deltaUV.y * edge.x - deltaUV.x * edge.y;
				tangent.y = deltaUV.y * edge.y - deltaUV.x * edge.x;
				tangent.z = deltaUV.y * edge.z - deltaUV.x * edge.z;
				tangent.Normalize();
			}

			Vector3f binormal;
			{
				binormal.x = deltaUV.x * edge.x - deltaUV.y * edge.y;
				binormal.y = deltaUV.x * edge.y - deltaUV.y * edge.x;
				binormal.z = deltaUV.x * edge.z - deltaUV.y * edge.z;
				binormal.Normalize();
			}

			// Assign tangent and binormal to the remaining vertices
			v0.tangent = tangent;
			v1.tangent = tangent;

			v0.binormal = binormal;
			v1.binormal = binormal;
		}
		else
		{
			// Two remaining vertices
			Vertex& v1 = vertices[i - 1];

			// Calculate tangent and binormal for the remaining vertices
			Vector3f edge = { v0.position.x - v1.position.x, v0.position.y - v1.position.y, v0.position.z - v1.position.z };
			Vector2f deltaUV = { v0.uv.x - v1.uv.x, v0.uv.y - v1.uv.y };

			Vector3f tangent;
			{
				tangent.x = deltaUV.y * edge.x - deltaUV.x * edge.y;
				tangent.y = deltaUV.y * edge.y - deltaUV.x * edge.x;
				tangent.z = deltaUV.y * edge.z - deltaUV.x * edge.z;
				tangent.Normalize();
			}

			Vector3f binormal;
			{
				binormal.x = deltaUV.x * edge.x - deltaUV.y * edge.y;
				binormal.y = deltaUV.x * edge.y - deltaUV.y * edge.x;
				binormal.z = deltaUV.x * edge.z - deltaUV.y * edge.z;
				binormal.Normalize();
			}

			v0.tangent = tangent;
			v0.binormal = binormal;
		}
	}
}

void GraphicsEngine::SetDepthStencilState(const DepthStencilState& aDepthStencilState)
{
	myContext->OMSetDepthStencilState(myDepthStencilStates[(int)aDepthStencilState].Get(), 0xffffffff);
}

void GraphicsEngine::SetCubemap(const std::string& someCubemapLocation)
{
	HRESULT result = DirectX::CreateDDSTextureFromFile(myDevice.Get(), StringHelper::s2ws(someCubemapLocation).c_str(), nullptr, &myCubemap);
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
	// Prepare FrameBufferData
	// This sets up the camera on the GPU
	FrameBufferData frameBufferData = {};
	frameBufferData.resolution = { (float)myWindowDimensions.x, (float)myWindowDimensions.y, (float)myViewportDimensions.x, (float)myViewportDimensions.y };
	frameBufferData.worldToClipMatrix = myCurrentCamera->GetProjectionMatrix();
	frameBufferData.modelToWorld = myCurrentCamera->GetModelMatrix();
	frameBufferData.cameraPosition = { myCurrentCamera->GetPosition().x, myCurrentCamera->GetPosition().y, myCurrentCamera->GetPosition().z, 0.0f };
	frameBufferData.nearPlane = myCurrentCamera->GetNearPlane();
	frameBufferData.farPlane = myCurrentCamera->GetFarPlane();

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	myContext->Map(myFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

	memcpy(mappedBuffer.pData, &frameBufferData, sizeof(FrameBufferData));

	myContext->Unmap(myFrameBuffer.Get(), 0);

	myContext->VSSetConstantBuffers((UINT)BufferSlots::Frame, 1, myFrameBuffer.GetAddressOf());
	myContext->PSSetConstantBuffers((UINT)BufferSlots::Frame, 1, myFrameBuffer.GetAddressOf());
}

void GraphicsEngine::SetBlendState(const BlendState& aBlendState)
{
	myContext->OMSetBlendState(myBlendStates[(int)aBlendState].Get(), nullptr, 0xffffffffu);
}

float GraphicsEngine::GetHeight(float x, float y)
{
	return 3 * (y * sin(0.1f * x) + x * cos(0.1f * y));
}

bool GraphicsEngine::CreateDepthStencilStates()
{
	HRESULT result = S_OK;
	D3D11_DEPTH_STENCIL_DESC desc = {};

	// Read only
	{
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		desc.StencilEnable = false;

		result = myDevice->CreateDepthStencilState(&desc, &myDepthStencilStates[(int)DepthStencilState::ReadOnly]);
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
		result = myDevice->CreateDepthStencilState(&desc, &myDepthStencilStates[(int)DepthStencilState::ReadOnlyGreater]);
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
		result = myDevice->CreateDepthStencilState(&desc, &myDepthStencilStates[(int)DepthStencilState::Disabled]);
		if (FAILED(result))
			return false;
	}

	// Read write
	{
		myDepthStencilStates[(int)DepthStencilState::ReadWrite] = nullptr;
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

		GraphicsEngine::GetInstance()->GetDevice()->CreateDepthStencilState(&desc, &myDepthStencilStates[(int)DepthStencilState::Additive]);

		HRESULT hr = GraphicsEngine::GetInstance()->GetDevice()->CreateDepthStencilState(&desc, &myAdditiveStencilState);
		if (FAILED(hr))
		{
			return false;
		}
	}

	return true;
}

bool GraphicsEngine::CreateRasterizerStates()
{
	HRESULT result = S_OK;

	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = D3D11_FILL_WIREFRAME;
	desc.CullMode = D3D11_CULL_BACK;
	desc.DepthClipEnable = true;

	result = myDevice->CreateRasterizerState(&desc, &myRasterizerStates[(int)RasterizerState::Wireframe]);
	if (FAILED(result))
		return false;

	desc.CullMode = D3D11_CULL_NONE;

	result = myDevice->CreateRasterizerState(&desc, &myRasterizerStates[(int)RasterizerState::WireframeNoCulling]);
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

	result = myDevice->CreateRasterizerState(&desc, &myRasterizerStates[(int)RasterizerState::NoFaceCulling]);
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

	result = myDevice->CreateRasterizerState(&desc, &myRasterizerStates[(int)RasterizerState::FrontfaceCulling]);
	if (FAILED(result))
		return false;

	myRasterizerStates[(int)RasterizerState::BackfaceCulling] = nullptr;

	return true;
}

void GraphicsEngine::SetRasterizerState(const RasterizerState& aRasterizerState)
{
	myContext->RSSetState(myRasterizerStates[(int)aRasterizerState].Get());
}

void GraphicsEngine::BeginFrame()
{
//#ifdef _DEBUG
	//myDrawCalls = 0;
//#endif
	UpdateFrameBuffer();
	{
		// Input Data
		InputBufferData inputData = {};
		inputData.time = (float)Engine::GetInstance()->GetTimer()->GetTotalTime();
		inputData.deltaTime = (float)Engine::GetInstance()->GetTimer()->GetDeltaTime();
		inputData.renderMode = myRenderMode;

		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		myContext->Map(myInputBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &inputData, sizeof(InputBufferData));
		myContext->Unmap(myInputBuffer.Get(), 0);

		myContext->VSSetConstantBuffers((UINT)BufferSlots::Input, 1, myInputBuffer.GetAddressOf());
		myContext->PSSetConstantBuffers((UINT)BufferSlots::Input, 1, myInputBuffer.GetAddressOf());
	}
	{
		//set sampler
		myContext->PSSetSamplers(0, 1, mySamplerState.GetAddressOf());
		myContext->PSSetSamplers(1, 1, my2dSamplerState.GetAddressOf());
		myContext->PSSetSamplers(2, 1, myClampSamplerState.GetAddressOf());
		myContext->PSSetShaderResources(0, 1, myCubemap.GetAddressOf());
	}

	{
		myContext->OMSetRenderTargets(1, myBackBufferRenderTarget->renderTargetView.GetAddressOf(), myDepthBuffer.GetDepthStencilView());
		{
			Vector4f color = { 0.02f, 0.02f, 0.02f, 1 }; //DARK GREY
			//color.x = myClearColor.x;
			//color.y = myClearColor.y;
			//color.z = myClearColor.z;

			myContext->ClearRenderTargetView(myBackBufferRenderTarget->renderTargetView.Get(), &color.x);

			color.x = 0.02f;
			color.y = 0.02f;
			color.z = 0.02f;
			myContext->ClearRenderTargetView(myBackBuffer.Get(), &color.x);
		}
		myContext->ClearDepthStencilView(myDepthBuffer.GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	
}

void GraphicsEngine::SetRawBackBufferAsRenderTarget()
{
	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), nullptr);
}

void GraphicsEngine::SetRawBackBufferAsRenderTarget(DepthBuffer* aDepth)
{
	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), aDepth->GetDepthStencilView());
}

void GraphicsEngine::EndFrame()
{
	mySwapChain->Present(1, 0);
}

void GraphicsEngine::PrepareForSpriteRender()
{
	//TO::DO  Set p? alpha igen 

	//TOVE: ALPHABLEND
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	myContext->OMSetRenderTargets(1, GetBackBuffer().GetAddressOf(), nullptr);
	myContext->OMSetBlendState(myAlphaBlendState.Get(), blendFactor, 0xFFFFFFFFu);
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

		result = GraphicsEngine::GetInstance()->GetDevice()->CreateBlendState(&blendStateDescription, myBlendStates[(int)BlendState::AdditiveBlend].ReleaseAndGetAddressOf());
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
		result = GraphicsEngine::GetInstance()->GetDevice()->CreateBlendState(&blendStateDescription, myBlendStates[(int)BlendState::Disabled].ReleaseAndGetAddressOf());
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
		result = GraphicsEngine::GetInstance()->GetDevice()->CreateBlendState(&blendStateDescription, myBlendStates[(int)BlendState::AlphaBlend].ReleaseAndGetAddressOf());





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
		//auto result = GraphicsEngine::GetInstance()->GetDevice()->CreateBlendState(&blendStateDescription, blendState.ReleaseAndGetAddressOf());






		if (FAILED(result))
		{
			return false;
		}

		return true;
	}
}

void GraphicsEngine::ResetToDefault()
{
	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), myDepthBuffer.GetDepthStencilView());
}

bool GraphicsEngine::SetResolution(const Vector2<int>& aResolution)
{
	aResolution;
	PrintW("[GraphicsEngine.cpp] SetResolution() is not fully implementet yet!");

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
	const D3D11_VIEWPORT* viewport = aViewport ? aViewport : myViewport;

	void* depth = myDepthBuffer.GetDepthStencilView();
	if (aDepth)
	{
		depth = aDepth->GetDepthStencilView();
	}

	myContext->OMSetRenderTargets(aNumViews, aViews[0].GetAddressOf(), (ID3D11DepthStencilView*)depth);
	myContext->RSSetViewports(1, viewport);
}

void GraphicsEngine::SetBackBufferAsActiveTarget(DepthBuffer* aDepth)
{
	void* depth = aDepth;
	if (aDepth)
	{
		depth = aDepth->GetDepthStencilView();
	}
	myContext->OMSetRenderTargets(1, GetBackBuffer().GetAddressOf(), (ID3D11DepthStencilView*)depth);
}

void GraphicsEngine::SetBackBufferAsActiveTarget()
{
	SetBackBufferAsActiveTarget(&myDepthBuffer);
}

void GraphicsEngine::IncrementRenderMode()
{
	myRenderMode = ++myRenderMode % (static_cast<unsigned int>(GBufferTexture::Count) + 1);
}

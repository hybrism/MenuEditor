#include "pch.h"
#include "PostProcess.h"

#include "Vertex.h"
#include "GraphicsData.h"
#include "../graphics/RenderTarget.h"
#include "../utility/Error.h"
#include "GraphicsEngine.h"

#include "../Paths.h"

PostProcess::PostProcess()
{
}

PostProcess::~PostProcess()
{
	delete myRenderTarget;
	delete myRenderTargetEffect;
	//delete myDepthStencilState;
	//delete myAdditiveStencilState;
	delete myBufferData;

	for (int i = 0; i < myDownScaledRenderTargets.size(); i++)
	{
		delete myDownScaledRenderTargets[i];
	}
}

void PostProcess::Init()
{
	auto device = GraphicsEngine::GetInstance()->GetDevice();
	//Vertex* vertices[4];
	std::vector<Vertex> vertices;
	//int amountOfVertices = 4;
	vertices.resize(4);

	vertices[0].position = Vector4<float>(-1.0f, -1.0f, 0.f, 1.f);
	vertices[0].uv = { 0,1 };
	vertices[1].position = { -1.0f,  1.0f,0.f,1.f };
	vertices[1].uv = { 0,0 };
	vertices[2].position = { 1.0f, -1.0f,0.f,1.f };
	vertices[2].uv = { 1,1 };
	vertices[3].position = { 1.0f,  1.0f,0.f,1.f };
	vertices[3].uv = { 1,0 };

	//int amountOfIndices = 6;
	unsigned int indices[6] =
	{
		2,1,0,
		3,2,1,
	};

	HRESULT result;
	//Create Vertex Buffer
	{
		D3D11_BUFFER_DESC vertexBufferDesc{};
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * 4;
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexData = { 0 };
		vertexData.pSysMem = vertices.data();
		result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);

		if (FAILED(result))
		{
			printf("Failed to create vertex buffer in " __FUNCSIG__ ".\n");
		}
	}
	//Create Index Buffer
	{
		D3D11_BUFFER_DESC indexBufferDesc{};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * 6;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA indexData = { 0 };
		indexData.pSysMem = indices;
		result = device->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);

		if (FAILED(result))
		{
			printf("Failed to create index buffer in " __FUNCSIG__ ".\n");
		}
	}

	std::string vsData;
	std::string file = "post_process_VS.CSO";
	//Load Shader
	{
		std::ifstream vsFile;
		vsFile.open(RELATIVE_SHADER_PATH + file, std::ios::binary);
		vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		result = device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &myVertexShader);
		if (FAILED(result))
		{
			PrintE("Could not load VS [PostProcess.cpp]");
		}
		vsFile.close();
	}
	//Pixel Shader
	{
		std::ifstream psFile;
		file = "post_process_PS.CSO";
		psFile.open(RELATIVE_SHADER_PATH + file, std::ios::binary);
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
		result = device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myPixelShader);
		if (FAILED(result))
		{
			PrintE("Could not load PS [PostProcess.cpp]");
		}
		psFile.close();
	}
	//TONEMAP Pixel Shader
	{
		std::ifstream psFile;
		file = "post_process_tone_map_PS.CSO";
		psFile.open(RELATIVE_SHADER_PATH + file, std::ios::binary);
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
		result = device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myToneMapPixelShader);
		if (FAILED(result))
		{
			PrintE("Could not load TONE MAP VS [PostProcess.cpp]");
		}
		psFile.close();
	}

	//TONEMAP Pixel Shader
	{
		std::ifstream psFile;
		file = "post_process_vignette_PS.CSO";
		psFile.open(RELATIVE_SHADER_PATH + file, std::ios::binary);
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
		result = device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myVingettePixelShader);
		if (FAILED(result))
		{
			PrintE("Could not load TONE MAP VS [PostProcess.cpp]");
		}
		psFile.close();
	}

	//Create Buffer
	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDescription.ByteWidth = sizeof(PostProcessBufferData);

	result = device->CreateBuffer(&bufferDescription, nullptr, &myPostProcessBuffer);
	if (FAILED(result))
	{
		printf("Failed to create CBuffer in " __FUNCSIG__ ".\n");
	}


	{
		// Create input layout
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		int numElements = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);
		result = device->CreateInputLayout(layout, numElements, vsData.data(), vsData.size(), &myInputLayout);
		if (FAILED(result))
		{
			PrintE("Could not create Input Layout [PostProcess.cpp]");
		}
	}

	{
		myRenderTarget = new RenderTarget;
		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = (int)GraphicsEngine::GetInstance()->GetBackTexturesize().x;
		desc.Height = (int)GraphicsEngine::GetInstance()->GetBackTexturesize().y;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		ID3D11Texture2D* texture;
		result = GraphicsEngine::GetInstance()->GetDevice()->CreateTexture2D(&desc, nullptr, &texture);
		assert(SUCCEEDED(result));
		result = GraphicsEngine::GetInstance()->GetDevice()->CreateShaderResourceView(texture, nullptr, &myRenderTarget->shaderResourceView);
		assert(SUCCEEDED(result));
		result = GraphicsEngine::GetInstance()->GetDevice()->CreateRenderTargetView(texture, nullptr, &myRenderTarget->renderTargetView);
		assert(SUCCEEDED(result));
		texture->Release();
		myRenderTarget->myDownScaleLevel = 0;
	}

	{
		myRenderTargetEffect = new RenderTarget;
		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = (int)GraphicsEngine::GetInstance()->GetBackTexturesize().x;
		desc.Height = (int)GraphicsEngine::GetInstance()->GetBackTexturesize().y;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		ID3D11Texture2D* texture;
		result = GraphicsEngine::GetInstance()->GetDevice()->CreateTexture2D(&desc, nullptr, &texture);
		assert(SUCCEEDED(result));
		result = GraphicsEngine::GetInstance()->GetDevice()->CreateShaderResourceView(texture, nullptr, &myRenderTargetEffect->shaderResourceView);
		assert(SUCCEEDED(result));
		result = GraphicsEngine::GetInstance()->GetDevice()->CreateRenderTargetView(texture, nullptr, &myRenderTargetEffect->renderTargetView);
		assert(SUCCEEDED(result));
		texture->Release();
		myRenderTargetEffect->myDownScaleLevel = 0;
	}

	static int divider = 2;
	// Create several rendertargets with different viewtargets for blur.
	for (int i = 1; i < 5; i++)
	{
		RenderTarget* temp = new RenderTarget;
		D3D11_TEXTURE2D_DESC descs;
		descs.Width = (int)(GraphicsEngine::GetInstance()->GetBackTexturesize().x / divider);
		descs.Height = (int)(GraphicsEngine::GetInstance()->GetBackTexturesize().y / divider);
		descs.MipLevels = 1;
		descs.ArraySize = 1;
		descs.Format = DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
		descs.SampleDesc.Count = 1;
		descs.SampleDesc.Quality = 0;
		descs.Usage = D3D11_USAGE_DEFAULT;
		descs.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		descs.CPUAccessFlags = 0;
		descs.MiscFlags = 0;
		ID3D11Texture2D* texturen;
		result = GraphicsEngine::GetInstance()->GetDevice()->CreateTexture2D(&descs, nullptr, &texturen);
		assert(SUCCEEDED(result));
		result = GraphicsEngine::GetInstance()->GetDevice()->CreateShaderResourceView(texturen, nullptr, &temp->shaderResourceView);
		assert(SUCCEEDED(result));
		result = GraphicsEngine::GetInstance()->GetDevice()->CreateRenderTargetView(texturen, nullptr, &temp->renderTargetView);
		assert(SUCCEEDED(result));
		texturen->Release();

		temp->myDownScaleLevel = divider;



		myDownScaledRenderTargets.push_back(temp);

		divider = divider * 2;


		myDownScaledRenderTargets[i - 1]->myViewport.TopLeftX = 0.0f;
		myDownScaledRenderTargets[i - 1]->myViewport.TopLeftY = 0.0f;
		myDownScaledRenderTargets[i - 1]->myViewport.Width = static_cast<float>(descs.Width);
		myDownScaledRenderTargets[i - 1]->myViewport.Height = static_cast<float>(descs.Height);
		myDownScaledRenderTargets[i - 1]->myViewport.MinDepth = 0.0f;
		myDownScaledRenderTargets[i - 1]->myViewport.MaxDepth = 1.0f;
	}

	myBufferData = new PostProcessBufferData;

	myBufferData->AlphaBlendLevel = 0;
	myBufferData->DownScaleLevel = myRenderTargetEffect->myDownScaleLevel;
	myBufferData->buffSaturation = mySaturation;
	myBufferData->buffExposure = myExposure;

	myBufferData->buffBlackPoint[0] = myBlackPoint[0];
	myBufferData->buffBlackPoint[1] = myBlackPoint[1];
	myBufferData->buffBlackPoint[2] = myBlackPoint[2];
	myBufferData->buffContrast[0] = myContrast[0];
	myBufferData->buffContrast[1] = myContrast[1];
	myBufferData->buffContrast[2] = myContrast[2];
	myBufferData->buffTint[0] = myTint[0];
	myBufferData->buffTint[1] = myTint[1];
	myBufferData->buffTint[2] = myTint[2];

	//Update Buffer
	UpdatePostProcessBuffer();

	CreateBlendStates();
}

void PostProcess::Update()
{

}

#if !USE_POST_PROCESSING
#pragma warning( push )
#pragma warning( disable : 4702)
#endif

void PostProcess::FirstFrame()
{
	auto graphicsEng = GraphicsEngine::GetInstance();

#if !USE_POST_PROCESSING
	graphicsEng->SetBackBufferAsActiveTarget();
	return;
#endif

#ifdef _DEBUG
	if (graphicsEng->GetRenderMode() != 0)
	{
		graphicsEng->SetBackBufferAsActiveTarget();
		return;
	}
#endif

	// TODO: Should fetch clear color from engine
	auto* context = graphicsEng->GetContext();
	auto clearColor = graphicsEng->GetClearColor();
	ClearShaderResources();
	//graphicsEng->SetBackBufferAsActiveTarget();

	context->OMSetRenderTargets(1, myRenderTargetEffect->renderTargetView.GetAddressOf(), nullptr);
	context->ClearRenderTargetView(myRenderTargetEffect->renderTargetView.Get(), &clearColor.x);


}

void PostProcess::Render()
{
	auto graphicsEng = GraphicsEngine::GetInstance();

#if !USE_POST_PROCESSING
	return;
#endif

#ifdef _DEBUG
	if (graphicsEng->GetRenderMode() != 0)
	{
		return;
	}
#endif

	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;

	auto context = graphicsEng->GetContext();
	D3D_PRIMITIVE_TOPOLOGY prevTopology;
	context->IAGetPrimitiveTopology(&prevTopology);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->IASetInputLayout(myInputLayout.Get());

	context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->VSSetShader(myVertexShader.Get(), nullptr, 0);
	context->PSSetShader(myPixelShader.Get(), nullptr, 0);

	UseAdditiveState(eRenderStates::eInactive);

	//SÄTTA UPP ORGINAL BILDEN FÖRST
	//Update Buffer
	myBufferData->DownScaleLevel = myDownScaledRenderTargets[0]->myDownScaleLevel;
	UpdatePostProcessBuffer();

	//Förbereda första bildens downscale
	context->OMSetRenderTargets(1, myDownScaledRenderTargets[0]->renderTargetView.GetAddressOf(), nullptr);
	context->PSSetShaderResources(11, 1, myRenderTargetEffect->shaderResourceView.GetAddressOf());
	context->RSSetViewports(1, &myDownScaledRenderTargets[0]->myViewport);
	context->DrawIndexed(6, 0, 0);
	ClearShaderResources();

	//ID3D11RenderTargetView* nullRTV = nullptr;
	//context->OMSetRenderTargets(1, &nullRTV, nullptr);
	//context->PSSetShaderResources(11, 1, myDownScaledRenderTargets[0]->shaderResourceView.GetAddressOf());

	//Update Buffer
	myBufferData->DownScaleLevel = myDownScaledRenderTargets[0]->myDownScaleLevel;
	UpdatePostProcessBuffer();



	//LOOPA STOR RESOLUTION TILL BILDER MED MINDRE
	for (int i = 1; i < myDownScaledRenderTargets.size(); i++)
	{
		//Sätta upp näste bild
		context->OMSetRenderTargets(1, myDownScaledRenderTargets[i]->renderTargetView.GetAddressOf(), nullptr);
		context->PSSetShaderResources(11, 1, myDownScaledRenderTargets[i - 1]->shaderResourceView.GetAddressOf());
		myBufferData->DownScaleLevel = myDownScaledRenderTargets[i]->myDownScaleLevel;

		//Update Buffer
		UpdatePostProcessBuffer();

		//Byta Viewport och rita
		context->RSSetViewports(1, &myDownScaledRenderTargets[i]->myViewport);
		context->DrawIndexed(6, 0, 0);
		ClearShaderResources();
	}


	//S�TT P� ALPHA OCH LOOPA LITEN TILL STOR
	UseAdditiveState(eRenderStates::eAlpha);
	for (int i = (int)myDownScaledRenderTargets.size() - 1; i >= 1; i--)
	{
		//Sätta upp näste bild
		context->OMSetRenderTargets(1, myDownScaledRenderTargets[i - 1]->renderTargetView.GetAddressOf(), nullptr);
		context->PSSetShaderResources(11, 1, myDownScaledRenderTargets[i]->shaderResourceView.GetAddressOf());

		//Update Buffer
		UpdatePostProcessBuffer();

		//Byta Viewport och rita
		context->RSSetViewports(1, &myDownScaledRenderTargets[i - 1]->myViewport);
		context->DrawIndexed(6, 0, 0);
		ClearShaderResources();
	}


	//TILLBAKA TILL STÖRSTA BILDEN OCH SISTA SETUPPEN
	context->OMSetRenderTargets(1, myRenderTargetEffect->renderTargetView.GetAddressOf(), nullptr);
	context->PSSetShaderResources(11, 1, myDownScaledRenderTargets[0]->shaderResourceView.GetAddressOf());

	//Update Buffer
	myBufferData->DownScaleLevel = myRenderTargetEffect->myDownScaleLevel;
	UpdatePostProcessBuffer();

	//Rita ut sista
	graphicsEng->GetContext()->RSSetViewports(1, &graphicsEng->GetViewPort());
	context->DrawIndexed(6, 0, 0);
	ClearShaderResources();

	context->PSSetShader(myVingettePixelShader.Get(), nullptr, 0);
	context->OMSetRenderTargets(1, myRenderTarget->renderTargetView.GetAddressOf(), nullptr);
	context->PSSetShaderResources(11, 1, myRenderTargetEffect->shaderResourceView.GetAddressOf());
	context->DrawIndexed(6, 0, 0);
	ClearShaderResources();

	// Ändra Rendertarget till orginal bilden och byta pixelshader och finish
	context->PSSetShader(myToneMapPixelShader.Get(), nullptr, 0);
	graphicsEng->SetBackBufferAsActiveTarget();
	context->PSSetShaderResources(11, 1, myRenderTarget->shaderResourceView.GetAddressOf());

	// Update Buffer
	myBufferData->DownScaleLevel = myRenderTargetEffect->myDownScaleLevel;
	UpdatePostProcessBuffer();

	// Inaktivera alpha och sista renderingen utav orginal bilden
	context->RSSetViewports(1, &graphicsEng->GetViewPort());
	UseAdditiveState(eRenderStates::eInactive);
	context->DrawIndexed(6, 0, 0);
	ClearShaderResources();

	context->DrawIndexed(6, 0, 0);
	context->IASetPrimitiveTopology(prevTopology);
}

#if !USE_POST_PROCESSING
#pragma warning( pop ) 
#endif

bool PostProcess::CreateBlendStates()
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

		result = GraphicsEngine::GetInstance()->GetDevice()->CreateBlendState(&blendStateDescription, myAdditiveBlendState.ReleaseAndGetAddressOf());
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
		result = GraphicsEngine::GetInstance()->GetDevice()->CreateBlendState(&blendStateDescription, myInactivatedBlendState.ReleaseAndGetAddressOf());
		if (FAILED(result))
		{
			return false;
		}

		//Alpha
		blendStateDescription = {};
		blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
		blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
		blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		result = GraphicsEngine::GetInstance()->GetDevice()->CreateBlendState(&blendStateDescription, myAlphaBlendState.ReleaseAndGetAddressOf());
		if (FAILED(result))
		{
			return false;
		}


		// Create depth-stencil State
		{
			D3D11_DEPTH_STENCIL_DESC desc;
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

			GraphicsEngine::GetInstance()->GetDevice()->CreateDepthStencilState(&desc, &myAdditiveStencilState);

			HRESULT hr = GraphicsEngine::GetInstance()->GetDevice()->CreateDepthStencilState(&desc, &myAdditiveStencilState);
			if (FAILED(hr))
			{
				return false;
			}
		}

		{
			D3D11_DEPTH_STENCIL_DESC desc;
			ZeroMemory(&desc, sizeof(desc));

			desc.DepthEnable = false;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			desc.StencilEnable = false;
			desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			desc.BackFace = desc.FrontFace;

			HRESULT hr = GraphicsEngine::GetInstance()->GetDevice()->CreateDepthStencilState(&desc, &myDepthStencilState);
			if (FAILED(hr))
			{
				return false;
			}
		}

		return true;
	}
}

void PostProcess::UseAdditiveState(eRenderStates aState)
{
	aState;
	switch (aState)
	{
	case eRenderStates::eAdditive:
	{
		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		UINT sampleMask = 0xffffffff;

		GraphicsEngine::GetInstance()->GetContext()->OMSetBlendState(myAdditiveBlendState.Get(), blendFactor, sampleMask);

		GraphicsEngine::GetInstance()->GetContext()->OMSetDepthStencilState(myAdditiveStencilState, 0);


		break;
	}
	case eRenderStates::eAlpha:
	{
		GraphicsEngine::GetInstance()->GetContext()->OMSetBlendState(myAlphaBlendState.Get(), nullptr, 0xffffffff);
		GraphicsEngine::GetInstance()->GetContext()->OMSetDepthStencilState(nullptr, 0);
		break;
	}
	case eRenderStates::eInactive:
	{
		GraphicsEngine::GetInstance()->GetContext()->OMSetBlendState(myInactivatedBlendState.Get(), nullptr, 0xffffffff);

		GraphicsEngine::GetInstance()->GetContext()->OMSetDepthStencilState(nullptr, 0);
		break;
	}
	}
}

void PostProcess::UpdatePostProcessBuffer()
{
	auto context = GraphicsEngine::GetInstance()->GetContext();
	D3D11_MAPPED_SUBRESOURCE mappedBuffer{};

	auto result = context->Map(myPostProcessBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	if (FAILED(result))
	{
		PrintE("Could not map PostProcessBuffer [PostProcess.cpp]");
	}

	PostProcessBufferData* data = (PostProcessBufferData*)mappedBuffer.pData;






	data->AlphaBlendLevel = myAlphaBlendLevel;

	data->DownScaleLevel = myRenderTargetEffect->myDownScaleLevel;
	data->buffSaturation = mySaturation;
	data->buffExposure = myExposure;

	data->buffBlackPoint[0] = myBlackPoint[0];
	data->buffBlackPoint[1] = myBlackPoint[1];
	data->buffBlackPoint[2] = myBlackPoint[2];
	data->buffContrast[0] = myContrast[0];
	data->buffContrast[1] = myContrast[1];
	data->buffContrast[2] = myContrast[2];
	data->buffTint[0] = myTint[0];
	data->buffTint[1] = myTint[1];
	data->buffTint[2] = myTint[2];


	context->Unmap(myPostProcessBuffer.Get(), 0);

	context->VSSetConstantBuffers(5, 1, myPostProcessBuffer.GetAddressOf());
	context->PSSetConstantBuffers(5, 1, myPostProcessBuffer.GetAddressOf());
}

void PostProcess::ImguiAdjustBloom()
{
	//static bool openBloom = false;

	//ImGui::Begin("FriendshipDebug");
	//ImGui::BeginMenuBar();
	//if (ImGui::BeginMenu("Edit"))
	//{
	//	if (ImGui::MenuItem("Open Bloom Settings"))
	//	{
	//		//Toggle
	//		openBloom = !openBloom;

	//	}
	//	ImGui::EndMenu();
	//}
	//ImGui::EndMenuBar();
	//ImGui::End();

	//if (openBloom)
	//{
	//	ImGui::Begin("Bloom Adjustments");

	//	ImGui::DragFloat("Alpha Blend", &myAlphaBlendLevel, 0.005f, 0.0f, 1.0f);

	//	ImGui::DragFloat("Saturation", &mySaturation, 0.005f, 0, 10.0f);

	//	ImGui::DragFloat("Exposure", &myExposure, 0.005f, 0, 10.0f);

	//	ImGui::DragFloat3("Contrast", myContrast, 0.005f, 0.f, 1.2f);

	//	ImGui::DragFloat3("Tint", myTint, 0.005f, 0.f, 5.f);

	//	ImGui::DragFloat3("BlackPoint", myBlackPoint, 0.005f, 0.f, 5.f);


	//	ImGui::End();
	//}
}

void PostProcess::ClearShaderResources()
{
	ID3D11ShaderResourceView* nullSRV = nullptr;
	GraphicsEngine::GetInstance()->GetContext()->PSSetShaderResources(11, 1, &nullSRV);
}
